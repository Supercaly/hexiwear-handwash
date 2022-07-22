from collections import Counter
import os
from time import time
import tensorflow as tf
import numpy as np
import csv


class LSTMGenerator(tf.keras.utils.Sequence):
    def __init__(self, file_paths, dim, split=1, batch_size=32, shuffle=True):
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

        X = np.empty((self.batch_size, 6, self.dim))
        Y = np.empty((self.batch_size), dtype=float)

        for i, file in enumerate(file_paths_tmp):
            with open(file, "r") as f:
                rows = list(csv.DictReader(f))

                if len(rows) != self.dim:
                    print(f"ho una size diversa {len(rows)}")
                    l = len(rows)
                    for _ in range(self.dim - l):
                        rows.append({"ax": 0.0, "ay": 0.0, "az": 0.0, "gx": 0.0,
                                    "gy": 0.0, "gz": 0.0, "label": 0.0, "subjectID": 0.0})

                X[i, 0, ] = [r["ax"] for r in rows]
                X[i, 1, ] = [r["ay"] for r in rows]
                X[i, 2, ] = [r["az"] for r in rows]
                X[i, 3, ] = [r["gx"] for r in rows]
                X[i, 4, ] = [r["gy"] for r in rows]
                X[i, 5, ] = [r["gz"] for r in rows]

                Y[i] = Counter([r['label'] for r in rows]).most_common(1)[0][0]

        return X, Y


def train_lstm(train_dir, test_dir, size, split):
    print(
        f"Training LSTM model with data from '{train_dir}' and '{test_dir}' and size of {size}.")
    print("This may take a while...")

    train_paths = [os.path.join(train_dir, f)
                   for f in os.listdir(train_dir) if f.endswith(".csv")]
    test_paths = [os.path.join(test_dir, f)
                  for f in os.listdir(test_dir) if f.endswith(".csv")]

    train_gen = LSTMGenerator(train_paths, size, split=split)
    test_gen = LSTMGenerator(test_paths, size, shuffle=False, split=split)

    # create the LSTM model to train
    model = tf.keras.Sequential()
    model.add(tf.keras.layers.Input(shape=(6, size)))
    model.add(tf.keras.layers.LSTM(256, return_sequences=True))
    model.add(tf.keras.layers.LSTM(128, return_sequences=True))
    model.add(tf.keras.layers.LSTM(85))
    # model.add(tf.keras.layers.Flatten())
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
              epochs=100,
              use_multiprocessing=True,
              workers=2)

    print(f"Training LSTM model done in {time()-start_time}s!")
    return model


def generate_lstm(train_dir, test_dir, ws, freq, split):
    model_path = "models"
    os.makedirs(model_path, exist_ok=True)
    model_path = os.path.join(model_path, f"lstm_{ws}")

    if not os.path.exists(model_path):
        model = train_lstm(train_dir, test_dir, ws*freq, split)

        model.save(model_path)
        print("Model saved!")

    converter = tf.lite.TFLiteConverter.from_saved_model(model_path)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.target_spec.supported_ops = [
        tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
    # allows conversion of MLIR operators.
    converter.experimental_new_converter = True
    converter.allow_custom_ops = True
    tflite_model = converter.convert()

    print("Model converted to TFLite!")

    with open(os.path.join(model_path, "model.tflite"), "wb+") as f:
        f.write(tflite_model)
