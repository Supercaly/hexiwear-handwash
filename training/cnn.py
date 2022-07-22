import csv
import numpy as np
import tensorflow as tf
from gaf import GramianAngularField
from collections import Counter
import os


def compute_cnn_input(chunk):
    """
    Compute che input and expected label for a CNN model.

    This function returns a tuple with two elements: 
        - a numpy array with 12 GAF images
        - the expected label
    """
    ax_gasf = GramianAngularField(method="summation")
    ax_gadf = GramianAngularField(method="difference")
    ay_gasf = GramianAngularField(method="summation")
    ay_gadf = GramianAngularField(method="difference")
    az_gasf = GramianAngularField(method="summation")
    az_gadf = GramianAngularField(method="difference")
    gx_gasf = GramianAngularField(method="summation")
    gx_gadf = GramianAngularField(method="difference")
    gy_gasf = GramianAngularField(method="summation")
    gy_gadf = GramianAngularField(method="difference")
    gz_gasf = GramianAngularField(method="summation")
    gz_gadf = GramianAngularField(method="difference")

    exp_label = Counter([c['label'] for c in chunk]).most_common(1)[0][0]
    gasf_ax = ax_gasf.transform(
        np.array([[float(c["ax"]) for c in chunk]]))
    gadf_ax = ax_gadf.transform(
        np.array([[float(c["ax"]) for c in chunk]]))
    gasf_ay = ay_gasf.transform(
        np.array([[float(c["ay"]) for c in chunk]]))
    gadf_ay = ay_gadf.transform(
        np.array([[float(c["ay"]) for c in chunk]]))
    gasf_az = az_gasf.transform(
        np.array([[float(c["az"]) for c in chunk]]))
    gadf_az = az_gadf.transform(
        np.array([[float(c["az"]) for c in chunk]]))
    gasf_gx = gx_gasf.transform(
        np.array([[float(c["gx"]) for c in chunk]]))
    gadf_gx = gx_gadf.transform(
        np.array([[float(c["gx"]) for c in chunk]]))
    gasf_gy = gy_gasf.transform(
        np.array([[float(c["gy"]) for c in chunk]]))
    gadf_gy = gy_gadf.transform(
        np.array([[float(c["gy"]) for c in chunk]]))
    gasf_gz = gz_gasf.transform(
        np.array([[float(c["gz"]) for c in chunk]]))
    gadf_gz = gz_gadf.transform(
        np.array([[float(c["gz"]) for c in chunk]]))

    images = np.array([gasf_ax[0], gadf_ax[0], gasf_ay[0], gadf_ay[0], gasf_az[0], gadf_az[0],
                       gasf_gx[0], gadf_gx[0], gasf_gy[0], gadf_gy[0], gasf_gz[0], gadf_gz[0]])
    images = np.moveaxis(images, 0, -1)

    return images, exp_label


class CNNGenerator(tf.keras.utils.Sequence):
    def __init__(self, file_paths, dim, split, batch_size=32, shuffle=True):
        self.file_paths = file_paths
        self.dim = dim
        self.split = split
        self.batch_size = batch_size
        self.shuffle = shuffle
        self.indexes = np.arange(len(self.file_paths))
        if self.shuffle == True:
            np.random.shuffle(self.indexes)

    def __len__(self):
        return int(len(self.file_paths) // self.batch_size) // self.split

    def on_epoch_end(self):
        self.indexes = np.arange(len(self.file_paths))
        if self.shuffle == True:
            np.random.shuffle(self.indexes)

    def __getitem__(self, index):
        indexes = self.indexes[index*self.batch_size:(index+1)*self.batch_size]

        file_paths_tmp = [self.file_paths[k] for k in indexes]

        X = np.empty((self.batch_size, (self.dim, self.dim, 12)))
        Y = np.empty((self.batch_size), dtype=float)

        for i, file in enumerate(file_paths_tmp):
            # print(f"open {i} {file}")
            with open(file, "r") as f:
                rows = csv.DictReader(f)
                x, y = compute_cnn_input(list(rows))
                X[i] = x
                Y[i] = y

        return X, Y


def train_cnn(train_dir, test_dir, size, split):
    """
    Perform the training process for a CNN model.
    """
    print(
        f"Training CNN model with fata from '{train_dir}' and '{test_dir}' and size of {size}.")
    print("This may take a while...")

    # first = next(train_input)
    train_paths = [os.path.join(train_dir, f)
                   for f in os.listdir(train_dir) if f.endswith(".csv")]
    test_paths = [os.path.join(test_dir, f)
                  for f in os.listdir(test_dir) if f.endswith(".csv")]

    train_gen = CNNGenerator(train_paths, size, split=split, batch_size=1)
    test_gen = CNNGenerator(test_paths, size, split=split,
                            shuffle=False, batch_size=1)

    # create the CNN model to train
    model = tf.keras.Sequential()
    model.add(tf.keras.layers.Input(shape=(size, size, 12)))
    model.add(tf.keras.layers.Conv2D(256, (5, 5)))
    model.add(tf.keras.layers.BatchNormalization(axis=1))
    model.add(tf.keras.layers.ReLU())
    model.add(tf.keras.layers.MaxPooling2D((3, 3)))

    model.add(tf.keras.layers.Conv2D(128, (3, 3)))
    model.add(tf.keras.layers.BatchNormalization(axis=1))
    model.add(tf.keras.layers.ReLU())
    model.add(tf.keras.layers.MaxPooling2D((2, 2)))

    model.add(tf.keras.layers.Conv2D(85, (2, 2)))
    model.add(tf.keras.layers.BatchNormalization(axis=1))
    model.add(tf.keras.layers.ReLU())
    model.add(tf.keras.layers.MaxPooling2D((2, 2)))

    model.add(tf.keras.layers.Conv2D(64, (2, 2)))
    model.add(tf.keras.layers.BatchNormalization(axis=1))
    model.add(tf.keras.layers.ReLU())

    model.add(tf.keras.layers.Flatten())
    model.add(tf.keras.layers.Dense(256))
    model.add(tf.keras.layers.Dense(128))
    model.add(tf.keras.layers.Dense(3))
    model.add(tf.keras.layers.Softmax())

    model.summary()

    model.compile(loss='sparse_categorical_crossentropy',
                  optimizer='adam', metrics=["sparse_categorical_accuracy"])

    # train the CNN model
    model.fit(x=train_gen,
              validation_data=test_gen,
              epochs=3,
              use_multiprocessing=True,
              workers=6)

    print("Training CNN model done!")
    return model


def generate_cnn(train_dir, test_dir, ws, freq, split):
    model_path = "models"
    os.makedirs(model_path, exist_ok=True)
    model_path = os.path.join(model_path, f"cnn_{ws}")

    if not os.path.exists(model_path):
        model = train_cnn(train_dir, test_dir, ws*freq, split)

        model.save(model_path)
        print("Model saved!")
