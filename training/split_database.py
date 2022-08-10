from collections import Counter
import csv
from itertools import islice
import os
from random import shuffle
import shutil
from statistics import mean, stdev
import click
import numpy as np


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


def load_raw_data(path):
    with open(path, "r") as f:
        rows = csv.DictReader(f)
        for r in rows:
            yield r


def compute_features(chunk):
    return [
        get_label_for_chunk(chunk, ""),  # "label":
        mean(list([float(x["ax"]) for x in chunk])),  # "avg_ax":
        mean(list([float(x["ay"]) for x in chunk])),  # "avg_ay":
        mean(list([float(x["az"]) for x in chunk])),  # "avg_az":
        mean(list([float(x["gx"]) for x in chunk])),  # "avg_gx":
        mean(list([float(x["gy"]) for x in chunk])),  # "avg_gy":
        mean(list([float(x["gz"]) for x in chunk])),  # "avg_gz":
        stdev(list([float(x["ax"]) for x in chunk])),  # "std_ax":
        stdev(list([float(x["ay"]) for x in chunk])),  # "std_ay":
        stdev(list([float(x["az"]) for x in chunk])),  # "std_az":
        stdev(list([float(x["gx"]) for x in chunk])),  # "std_gx":
        stdev(list([float(x["gy"]) for x in chunk])),  # "std_gy":
        stdev(list([float(x["gz"]) for x in chunk])),  # "std_gz":
        max(list([float(x["ax"]) for x in chunk])),  # "max_ax":
        max(list([float(x["ay"]) for x in chunk])),  # "max_ay":
        max(list([float(x["az"]) for x in chunk])),  # "max_az":
        max(list([float(x["gx"]) for x in chunk])),  # "max_gx":
        max(list([float(x["gy"]) for x in chunk])),  # "max_gy":
        max(list([float(x["gz"]) for x in chunk])),  # "max_gz":
        min(list([float(x["ax"]) for x in chunk])),  # "min_ax":
        min(list([float(x["ay"]) for x in chunk])),  # "min_ay":
        min(list([float(x["az"]) for x in chunk])),  # "min_az":
        min(list([float(x["gx"]) for x in chunk])),  # "min_gx":
        min(list([float(x["gy"]) for x in chunk])),  # "min_gy":
        min(list([float(x["gz"]) for x in chunk]))  # "min_gz":
    ]


def subset(features):
    print("subset")
    f0 = list(filter(lambda e: e[0] == 0, features))
    f1 = list(filter(lambda e: e[0] == 1, features))
    f2 = list(filter(lambda e: e[0] == 2, features))
    shuffle(f0)
    shuffle(f1)
    shuffle(f2)
    f2 = f2[:200]
    f1 = f1[:200]
    f0 = f0[:200]
    print(f"0: {len(f0)} 1: {len(f1)} 2: {len(f2)}")
    f = f0+f1+f2
    shuffle(f)
    return f


@click.command()
@click.argument("src", type=click.Path(dir_okay=False, exists=True))
@click.option("--mode", type=click.Choice(["svm", "knn", "cnn", "lstm"], case_sensitive=False), required=True, help="Select the Machine Learning model type to work with.")
@click.option("-w", "--window_size", type=int, default=10, show_default=True, help="Size of the window to apply.")
@click.option("-f", "--freq", type=int, default=100, show_default=True, help="Sampling frequency in Hz.")
@click.option("-d", "--dst", type=click.Path(file_okay=False), default="data", show_default=True, help="Folder where to save generated data. This will contain a sub-folder named as the window_size.")
@click.option("-s", "--percent", type=click.FloatRange(0.0, 1.0), default=0.80, show_default=True, help="Percentage of data put into the training set compared to that in the test set.")
def main(src, mode, window_size, freq, dst, percent):
    """Loads the database .csv file and split it indo training and
    testing sets.
    """
    dst = os.path.join(dst, str(window_size))

    # load raw data
    raw_data = iter(load_raw_data(src))

    # ensure destination directories
    train_dir = dst
    test_dir = dst
    os.makedirs(train_dir, exist_ok=True)
    os.makedirs(test_dir, exist_ok=True)

    print(
        f"Splitting database '{src}' in '{train_dir}' and '{test_dir}' with {percent*100}% split.")
    print("This may take some time...")

    if mode == "svm":
        train_file = open(os.path.join(train_dir, "train.csv"), "w+")
        test_file = open(os.path.join(test_dir, "test.csv"), "w+")

        feats = []
        if not os.path.exists(os.path.join(train_dir, "data.csv")):
            # split raw data by window_size
            chunk = list(islice(raw_data, window_size * freq))
            while chunk:
                feats.append(compute_features(chunk))
                chunk = list(islice(raw_data, window_size * freq))
            with open(os.path.join(train_dir, "data.csv"), "w+") as d:
                w = csv.writer(d)
                w.writerows(feats)
        else:
            with open(os.path.join(train_dir, "data.csv"), "r") as d:
                feats = list(csv.reader(d))
                feats = [[int(ff) if i == 0 else float(ff)
                          for i, ff in enumerate(f)] for f in feats]

        feats = subset(feats)
        for feat in feats:
            pattern = ""
            for i, f in enumerate(feat):
                if i != 0:
                    pattern += f" {i}:"
                pattern += f"{f}"
            pattern += "\n"
            if np.random.uniform() < percent:
                # write to training set
                train_file.write(pattern)
            else:
                # write to test set
                test_file.write(pattern)

    else:
        train_dir = os.path.join(dst, "train")
        test_dir = os.path.join(dst, "test")
        os.makedirs(train_dir, exist_ok=True)
        os.makedirs(test_dir, exist_ok=True)

        # split raw data by window_size
        chunk = list(islice(raw_data, window_size * freq))

        # write chunks in training file or test file
        # a chunk has 80% probability of going in the
        # training set adn 20% probability in test set.
        cnt = 0
        file_map = {"0": [], "1": [], "2": []}

        while chunk:
            if len(chunk) == window_size*freq:
                label = get_label_for_chunk(chunk, "")

                if np.random.uniform() < percent:
                    file_path = os.path.join(train_dir, f"{cnt}.csv")
                else:
                    file_path = os.path.join(test_dir, f"{cnt}.csv")
                # file_map[str(label)].append(file_path)
                # cnt += 1

                with open(file_path, "w+") as f:
                    w = csv.DictWriter(f, chunk[0].keys())
                    w.writeheader()
                    w.writerows(chunk)
            cnt +=1
            chunk = list(islice(raw_data, window_size * freq))

        # m = min(len(file_map["0"]), min(
        #     len(file_map["1"]), len(file_map["2"])))
        # shuffle(file_map["0"])
        # a = file_map["0"][m:]
        # file_map["0"] = file_map["0"][:m]
        # for f in a:
        #     os.remove(f)

        # for f in file_map["0"]:
        #     if np.random.uniform() > percent:
        #         shutil.move(f, os.path.join(test_dir, os.path.basename(f)))
        # for f in file_map["1"]:
        #     if np.random.uniform() > percent:
        #         shutil.move(f, os.path.join(test_dir, os.path.basename(f)))
        # for f in file_map["2"]:
        #     if np.random.uniform() > percent:
        #         shutil.move(f, os.path.join(test_dir, os.path.basename(f)))


if __name__ == "__main__":
    main()
