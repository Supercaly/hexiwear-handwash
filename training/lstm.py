from collections import Counter
import os
import sys
import tempfile
from time import time
from matplotlib import pyplot as plt
import tensorflow as tf
import numpy as np
import csv
import tensorflow_model_optimization as tfmot
import lstm_metrics


def get_label_for_chunk(chunk, f):
    l = {"0": 0, "1": 0, "2": 0}
    for c in chunk:
        l[c["label"]] += 1
    if l["0"] >= l["1"] and l["0"] >= l["2"]:
        return 0
    elif l["1"] >= l["0"] and l["1"] >= l["2"]:
        return 1
    elif l["2"] >= l["0"] and l["2"] >= l["1"]:
        return 2
    else:
        raise Exception(f"get_label_for_chunk: wrong label {l} {f}")


class LSTMGenerator(tf.keras.utils.Sequence):
    def __init__(self, file_paths, dim, split=1, batch_size=32, shuffle=True):
        self.file_paths = file_paths
        self.dim = dim
        self.split = split
        self.batch_size = batch_size
        self.shuffle = shuffle
        self.indexes = np.arange(len(self.file_paths))

    def __len__(self):
        return int(len(self.file_paths) // self.batch_size) // self.split

    def on_epoch_end(self):
        self.indexes = np.arange(len(self.file_paths))
        if self.shuffle == True:
            np.random.shuffle(self.indexes)

    def __getitem__(self, index):
        indexes = self.indexes[index*self.batch_size:(index+1)*self.batch_size]

        file_paths_tmp = [self.file_paths[k] for k in indexes]

        X = np.empty((self.batch_size, 6, self.dim), dtype=float)
        Y = np.empty((self.batch_size), dtype=float)

        csv.field_size_limit(sys.maxsize)
        for i, file in enumerate(file_paths_tmp):
            with open(file, "r") as f:
                rows = list(csv.DictReader(f))

                if len(rows) != self.dim:
                    raise Exception(f"ho una size diversa {len(rows)}")

                X[i, 0, ] = [float(r["ax"]) for r in rows]
                X[i, 1, ] = [float(r["ay"]) for r in rows]
                X[i, 2, ] = [float(r["az"]) for r in rows]
                X[i, 3, ] = [float(r["gx"]) for r in rows]
                X[i, 4, ] = [float(r["gy"]) for r in rows]
                X[i, 5, ] = [float(r["gz"]) for r in rows]

                Y[i] = get_label_for_chunk(rows, file)
        a = tf.keras.utils.to_categorical(Y, num_classes=3)

        return X, a


def get_data(files, size):
    X = np.zeros((len(files), 6, size), dtype=np.float32)
    Y = np.zeros((len(files)))
    print(X.shape)
    for i, f in enumerate(files):
        with open(f, "r") as c:
            rows = list(csv.DictReader(c))
            X[i, 0, ] = [float(r["ax"]) for r in rows]
            X[i, 1, ] = [float(r["ay"]) for r in rows]
            X[i, 2, ] = [float(r["az"]) for r in rows]
            X[i, 3, ] = [float(r["gx"]) for r in rows]
            X[i, 4, ] = [float(r["gy"]) for r in rows]
            X[i, 5, ] = [float(r["gz"]) for r in rows]
            Y[i] = get_label_for_chunk(rows, f)
    # Y = tf.keras.utils.to_categorical(Y, num_classes=3)
    return X, Y


def get_data2(file, size):
    with open(file, "r") as c:
        rows = list(csv.DictReader(c))
        cs = len(rows) // size
        X = np.zeros((cs, 6, size))
        Y = np.zeros((cs))
        i = 0
        for x in range(0, len(rows), size):
            chunk = rows[x:size+x]
            X[i, 0, ] = [float(r["ax"]) for r in chunk]
            X[i, 1, ] = [float(r["ay"]) for r in chunk]
            X[i, 2, ] = [float(r["az"]) for r in chunk]
            X[i, 3, ] = [float(r["gx"]) for r in chunk]
            X[i, 4, ] = [float(r["gy"]) for r in chunk]
            X[i, 5, ] = [float(r["gz"]) for r in chunk]
            Y[i] = get_label_for_chunk(chunk, "f")
            i += 1
    Y = tf.keras.utils.to_categorical(Y, num_classes=3)
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
    # train_x, train_y = get_data(train_paths, size)
    # test_x, test_y = get_data(test_paths, size)
    # d, l = get_data2("data/2/t.csv", size)
    # t_x = d[:int(len(d)*.8)]
    # t_y = l[:int(len(d)*.8)]
    # v_x = d[int(len(d)*.8):]
    # v_y = l[int(len(d)*.8):]

    # print(f"{train_x.shape} {train_y.shape} {test_x.shape} {test_y.shape}")

    # create the LSTM model to train
    model = tf.keras.Sequential()
    model.add(tf.keras.layers.Input(shape=(6, size)))
    model.add(tf.keras.layers.LSTM(8, unroll=True))
    model.add(tf.keras.layers.Dense(32))
    model.add(tf.keras.layers.Dense(3))
    model.add(tf.keras.layers.Softmax())

    model.summary()

    model.compile(loss='categorical_crossentropy',
                  optimizer='adam', metrics=["accuracy"])

    start_time = time()

    # train the CNN model
    history = model.fit(x=train_gen,
                        validation_data=test_gen,
                        epochs=1,
                        use_multiprocessing=True,
                        workers=1)

    print(f"Training LSTM model done in {time()-start_time}s!")

    return model


def generate_lstm(train_dir, test_dir, ws, freq, split, force):
    model_path = "models"
    os.makedirs(model_path, exist_ok=True)
    model_path = os.path.join(model_path, f"lstm_{ws}")

    if force or not os.path.exists(model_path):
        model = train_lstm(train_dir, test_dir, ws*freq, split)
        model.save(model_path)
        print("Model saved!")

    print("Converting model to TFLite...")

    def rep_dataset_gen():
        p = [os.path.join(test_dir, f)
             for f in os.listdir(test_dir) if f.endswith(".csv")]
        p = p[:100]
        for i, file in enumerate(p):
            with open(file, "r") as f:
                rows = list(csv.DictReader(f))
                X = np.zeros((6, ws*freq))
                X[0, ] = [r["ax"] for r in rows]
                X[1, ] = [r["ay"] for r in rows]
                X[2, ] = [r["az"] for r in rows]
                X[3, ] = [r["gx"] for r in rows]
                X[4, ] = [r["gy"] for r in rows]
                X[5, ] = [r["gz"] for r in rows]
                yield [np.array(X, dtype=np.float32, ndmin=3)]

    converter = tf.lite.TFLiteConverter.from_saved_model(model_path)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.post_training_quantize = True
    # converter.target_spec.supported_ops = [
    #     tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
    # converter.experimental_new_converter = True
    # converter.allow_custom_ops = True
    # converter.target_spec.supported_ops = [
    #     tf.lite.OpsSet.TFLITE_BUILTINS_INT8,
    #     tf.lite.OpsSet.SELECT_TF_OPS]
    # converter.experimental_enable_resource_variables = True
    converter.representative_dataset = rep_dataset_gen
    converter.inference_input_type = tf.int8  # or tf.uint8
    converter.inference_output_type = tf.int8  # or tf.uint8

    tflite_model = converter.convert()

    print("Model converted to TFLite!")

    with open(os.path.join(model_path, "model2.tflite"), "wb+") as f:
        f.write(tflite_model)

    # lstm_metrics.run(ws, "models", "data")
