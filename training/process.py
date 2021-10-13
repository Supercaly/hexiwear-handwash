import statistics
import os
import numpy as np
import tensorflow as tf
import time

from utensor_cgen.api.export import tflm_keras_export

def compute_features(file_path, overlap = 1):
    assert(overlap >= 0.0 and overlap <= 1.0)
    print("Computing features from data...")

    features_val = []
    with open(file_path, "r") as file:
        lines = file.readlines()[1:]

        idx = 0
        while idx < len(lines):
            current_rows = lines[idx: idx+1000]
            

            current_rows = [x.rstrip("\n").split(',')[1:] for x in current_rows]
            current_rows = [[float(x) if '.' in x else int(x) for x in xs] for xs in current_rows]
            current_cols = list(map(list,zip(*current_rows)))

            avgs = tuple([statistics.mean(x) for x in current_cols[:6]])
            stds = tuple([statistics.stdev(x) for x in current_cols[:6]])
            maxes = tuple([max(x) for x in current_cols[:6]])
            mins = tuple([min(x) for x in current_cols[:6]])
            state = 0 if statistics.mean(current_cols[6]) < 0.5 else 1
            hand = 0 if statistics.mean(current_cols[7]) < 0.5 else 1
            features_val.append([
                avgs[0],avgs[1],avgs[2],avgs[3],avgs[4],avgs[5],
                stds[0],stds[1],stds[2],stds[3],stds[4],stds[5],
                maxes[0],maxes[1],maxes[2],maxes[3],maxes[4],maxes[5],
                mins[0],mins[1],mins[2],mins[3],mins[4],mins[5],
                hand,
                state
            ])
            
            idx += int(1000*overlap)
    print("Computing features from data end!")
    return features_val


#input_file_path = "../../../Raccolta Dati/export/lorenzotracce_corte_left_1.csv"
input_file_path = "../../../Raccolta Dati/export/sum.csv"

start_time = time.time()

# check if computed features are stored in a file
if os.path.isfile("./comp_feats.txt"):
    with open("./comp_feats.txt", "r") as f:
        feats = [[int(e) if i == 24 or i == 25 else float(e) for i, e in enumerate(l[:-1].split(", "))] for l in f.readlines()]
        data = np.array(feats)
else:
    feats = compute_features(input_file_path,0.5)
    with open("./comp_feats.txt", "w+") as f:
        [f.write(", ".join(str(e) for e in l)+"\n") for l in feats]
    data = np.array(feats)

np.random.shuffle(data)
split_idx = int(data.shape[0] * 0.8)
training, test = data[:split_idx, :], data[split_idx:, :]
x_train, y_train = training[:, :-1], training[:, -1]
x_test, y_test = test[:, :-1], test[:, -1]
print("")
print("training shape:", x_train.shape)
print("test shape:", x_test.shape)
print("")

model = tf.keras.Sequential([
    tf.keras.layers.Dense(256, input_dim=25, activation="relu"),
    tf.keras.layers.Dense(256, activation="relu"),
    tf.keras.layers.Dense(3, activation="softmax")
])
model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=["accuracy"])

model.fit(x_train, y_train, epochs=100, batch_size=32)

loss, accuracy = model.evaluate(x_test, y_test)
print("loss: {}, accuracy: {}".format(loss, accuracy))

x_check, y_check = x_test[:20], y_test[:20]
predictions = model.predict(x_check)

print("Test the prediction of {} samples".format(len(predictions)))
for i in range(len(y_check)):
    print("  got: {} [{}], expected: {}".format(predictions[i], np.argmax(predictions[i]), y_check[i]))

print("Complete in {}".format(time.time()-start_time))

print("Saving model...")

def rep_dataset_gen():
    for _ in range(128):
        idx = np.random.randint(0, x_test.shape[0]-1)
        sample = x_test[idx]
        sample = sample[tf.newaxis, ...]
        sample = tf.cast(sample, dtype=tf.float32)
        yield [sample]

model.save("mlp_handwash_model")
tflm_keras_export(
    "mlp_handwash_model", 
    model_name="mlp_hw_model", 
    representive_dataset=rep_dataset_gen)

print("Model saved!")