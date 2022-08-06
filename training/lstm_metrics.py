import click
import os
import csv
import tensorflow as tf
import numpy as np
from collections import Counter
import sys

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

@click.command()
@click.option("-w", "--window", type=int, required=True, help="Select the window size")
@click.option("-m", "--model_path", type=click.Path(file_okay=False), default="models", show_default=True, help="Path to the model dir to use")
@click.option("-d", "--test_path", type=click.Path(file_okay=False), default="data", show_default=True, help="Path to the data dir to use")
def main(window, model_path, test_path):
    run(window, model_path, test_path)

def run(window, model_path, test_path):
    size = window * 100
    model_path = os.path.join(model_path, f"lstm_{window}/model.tflite")
    test_path = os.path.join(test_path, f"{window}/test")

    test_paths = [os.path.join(test_path, f)
                  for f in os.listdir(test_path) if f.endswith(".csv")]

    interpreter = tf.lite.Interpreter(model_path)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()
    confusion_matrix = [0]*9

    csv.field_size_limit(sys.maxsize)
    for test_file in test_paths:
        with open(test_file, "r") as f:
            chunk = list(csv.DictReader(f))
            if len(chunk) != size:
                continue
            actual_label = get_label_for_chunk(chunk, "aa")

            data = np.array([[[np.float32(d["ax"]) for d in chunk],
                              [np.float32(d["ay"]) for d in chunk],
                              [np.float32(d["az"]) for d in chunk],
                              [np.float32(d["gx"]) for d in chunk],
                              [np.float32(d["gy"]) for d in chunk],
                              [np.float32(d["gz"]) for d in chunk]]])
        interpreter.set_tensor(input_details[0]['index'], data)
        interpreter.invoke()

        out = interpreter.get_tensor(output_details[0]["index"])
        # print(out)
        predicted_label = np.argmax(out)
        # print(predicted_label)
        # print(actual_label)
        confusion_matrix[3*actual_label + predicted_label] += 1

    print(confusion_matrix)
    m = ""
    for i in range(3):
        for j in range(3):
            m += f"{confusion_matrix[3*i + j]} "
        print(m)
        m = ""

    tp = [0, 0, 0]
    tn = [0, 0, 0]
    fp = [0, 0, 0]
    fn = [0, 0, 0]

    tp[0] = confusion_matrix[0]
    tn[0] = confusion_matrix[4] + confusion_matrix[5] + \
        confusion_matrix[7] + confusion_matrix[8]
    fp[0] = confusion_matrix[3] + confusion_matrix[6]
    fn[0] = confusion_matrix[1] + confusion_matrix[2]

    tp[1] = confusion_matrix[4]
    tn[1] = confusion_matrix[0] + confusion_matrix[2] + \
        confusion_matrix[6] + confusion_matrix[8]
    fp[1] = confusion_matrix[1] + confusion_matrix[7]
    fn[1] = confusion_matrix[3] + confusion_matrix[5]

    tp[2] = confusion_matrix[8]
    tn[2] = confusion_matrix[0] + confusion_matrix[1] + \
        confusion_matrix[3] + confusion_matrix[4]
    fp[2] = confusion_matrix[2] + confusion_matrix[5]
    fn[2] = confusion_matrix[6] + confusion_matrix[7]

    print(f"Class 0")
    print(f"  tp: {tp[0]} tn: {tn[0]} fp: {fp[0]} fn: {fn[0]}")
    print(f"Class 1")
    print(f"  tp: {tp[1]} tn: {tn[1]} fp: {fp[1]} fn: {fn[1]}")
    print(f"Class 2")
    print(f"  tp: {tp[2]} tn: {tn[2]} fp: {fp[2]} fn: {fn[2]}")

    precision = 0.0
    recall = 0.0
    f1 = 0.0
    accuracy = 0.0

    for i in range(3):
        try:
            precision += tp[i] / (tp[i] + fp[i])
        except:
            pass
        try:
            recall += tp[i] / (tp[i] + fn[i])
        except:
            pass
        try:
            accuracy += (tp[i] + tn[i]) / (tp[i] + tn[i] + fp[i] + fn[i])
        except:
            pass
    
    precision /= 3.0
    recall /= 3.0
    accuracy /= 3.0
    try:
        f1 = 2 * ((precision * recall) / (precision + recall))
    except:
        pass

    print(f"Precision: {precision}")
    print(f"Recall: {recall}")
    print(f"F1 score:{ f1}")
    print(f"Accuracy: {accuracy}")

if __name__ == "__main__":
    main()
