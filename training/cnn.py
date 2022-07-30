import csv
import tempfile
from time import time
import numpy as np
import tensorflow as tf
from gaf import GramianAngularField
from collections import Counter
import os
import tensorflow_model_optimization as tfmot


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
        l = int(len(self.file_paths) // self.batch_size) // self.split
        print(f"len: {l}")
        return l

    def on_epoch_end(self):
        self.indexes = np.arange(len(self.file_paths))
        if self.shuffle == True:
            np.random.shuffle(self.indexes)

    def __getitem__(self, index):
        indexes = self.indexes[index*self.batch_size:(index+1)*self.batch_size]

        file_paths_tmp = [self.file_paths[k] for k in indexes]

        X = np.empty((self.batch_size, self.dim, self.dim, 12))
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

    start_time = time()

    # train the CNN model
    model.fit(x=train_gen,
              validation_data=test_gen,
              epochs=1,
              use_multiprocessing=True,
              workers=6)

    print(f"Training CNN model done in {time() - start_time}s!")
    return model


def prune_cnn(model, train_dir, test_dir, size, split):
    prune_low_magnitude = tfmot.sparsity.keras.prune_low_magnitude

    train_paths = [os.path.join(train_dir, f)
                   for f in os.listdir(train_dir) if f.endswith(".csv")]
    test_paths = [os.path.join(test_dir, f)
                  for f in os.listdir(test_dir) if f.endswith(".csv")]
    # num_images = train_images.shape[0] * (1 - validation_split)
    end_step = np.ceil(len(train_paths) / split).astype(np.int32)

    # Define model for pruning.
    pruning_params = {
        'pruning_schedule': tfmot.sparsity.keras.PolynomialDecay(initial_sparsity=0.50,
                                                                 final_sparsity=0.80,
                                                                 begin_step=0,
                                                                 end_step=end_step)
    }

    model_for_pruning = prune_low_magnitude(model, **pruning_params)

    # `prune_low_magnitude` requires a recompile.
    model_for_pruning.compile(optimizer='adam',
                              loss=tf.keras.losses.SparseCategoricalCrossentropy(
                                  from_logits=True),
                              metrics=['accuracy'])

    model_for_pruning.summary()

    train_gen = CNNGenerator(train_paths, size, split=split, batch_size=1)
    test_gen = CNNGenerator(test_paths, size, split=split,
                            shuffle=False, batch_size=1)
    logdir = tempfile.mkdtemp()

    callbacks = [
    tfmot.sparsity.keras.UpdatePruningStep(),
    tfmot.sparsity.keras.PruningSummaries(log_dir=logdir),
    ]

    model_for_pruning.fit(train_gen, batch_size=1, epochs=1, validation_data=test_gen,
                    callbacks=callbacks)
    model_for_export = tfmot.sparsity.keras.strip_pruning(model_for_pruning)
    
    print("Model Pruned!")
    return model_for_export

def generate_cnn(train_dir, test_dir, ws, freq, split):
    model_path = "models"
    os.makedirs(model_path, exist_ok=True)
    model_path = os.path.join(model_path, f"cnn_{ws}")

    if not os.path.exists(model_path):
        model = train_cnn(train_dir, test_dir, ws*freq, split)
        model = prune_cnn(model, train_dir, test_dir, ws*freq, split)

        model.save(model_path)
        print("Model saved!")

    def rep_dataset_gen():
        p = [os.path.join(test_dir, f)
             for f in os.listdir(test_dir) if f.endswith(".csv")]
        p = p[:100]
        for i, file in enumerate(p):
            with open(file, "r") as f:
                rows = list(csv.DictReader(f))
                X, _ = compute_cnn_input(rows)
                yield [np.array(X, dtype=np.float32, ndmin=4)]

    print("Converting CNN model to TFLite...")
    converter = tf.lite.TFLiteConverter.from_saved_model(model_path)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]

    converter.representative_dataset = rep_dataset_gen
    # Ensure that if any ops can't be quantized, the converter throws an error
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    # Set the input and output tensors to uint8 (APIs added in r2.3)
    converter.inference_input_type = tf.uint8
    converter.inference_output_type = tf.uint8
    tflite_model = converter.convert()

    print("Model converted to TFLite!")

    with open(os.path.join(model_path, "model.tflite"), "wb+") as f:
        f.write(tflite_model)
