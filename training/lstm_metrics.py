from email.policy import default
from traceback import print_tb
import click
import os
import csv
from sklearn.metrics import confusion_matrix
import tensorflow as tf
import numpy as np
from collections import Counter
import sys

@click.command()
@click.option("-w", "--window", type=int, required=True, help="Select the window size")
@click.option("-m", "--model_path", type=click.Path(file_okay=False), default="models", show_default=True, help="Path to the model dir to use")
@click.option("-d", "--test_path", type=click.Path(file_okay=False), default="data", show_default=True, help="Path to the data dir to use")
def main(window, model_path, test_path):
    size = window * 100
    model_path = os.path.join(model_path, f"lstm_{window}/model.tflite")
    test_path = os.path.join(test_path, f"{window}/train")

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
            actual_label = int(float(Counter([r['label']
                                              for r in chunk]).most_common(1)[0][0]))

            data = np.array([[[np.int8(float(d["ax"])) for d in chunk],
                              [np.int8(float(d["ay"])) for d in chunk],
                              [np.int8(float(d["az"])) for d in chunk],
                              [np.int8(float(d["gx"])) for d in chunk],
                              [np.int8(float(d["gy"])) for d in chunk],
                              [np.int8(float(d["gz"])) for d in chunk]]])
        interpreter.set_tensor(input_details[0]['index'], data)
        interpreter.invoke()

        out = interpreter.get_tensor(output_details[0]["index"])
        predicted_label = np.argmax(out)
        confusion_matrix[3*actual_label + predicted_label] += 1

    print(confusion_matrix)

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

    print(f"Class 0\n")
    print(f"  tp: {tp[0]} tn: {tn[0]} fp: {fp[0]} fn: {fn[0]}\n")
    print(f"Class 1\n")
    print(f"  tp: {tp[1]} tn: {tn[1]} fp: {fp[1]} fn: {fn[1]}\n")
    print(f"Class 2\n")
    print(f"  tp: {tp[2]} tn: {tn[2]} fp: {fp[2]} fn: {fn[2]}\n")

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

    print(f"Precision: {precision}\n")
    print(f"Recall: {recall}\n")
    print(f"F1 score:{ f1}\n")
    print(f"Accuracy: {accuracy}\n")

if __name__ == "__main__":
    main()
