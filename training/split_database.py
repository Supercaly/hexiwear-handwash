from collections import Counter
import csv
from itertools import islice
import os
from statistics import mean, stdev
import click
import numpy as np

def load_raw_data(path):
    with open(path, "r") as f:
        rows = csv.DictReader(f)
        for r in rows:
            yield r

def compute_features(chunk):
    return {
        "avg_ax": mean(list([float(x["ax"]) for x in chunk])),
        "avg_ay": mean(list([float(x["ay"]) for x in chunk])),
        "avg_az": mean(list([float(x["az"]) for x in chunk])),
        "avg_gx": mean(list([float(x["gx"]) for x in chunk])),
        "avg_gy": mean(list([float(x["gy"]) for x in chunk])),
        "avg_gz": mean(list([float(x["gz"]) for x in chunk])),
        "std_ax": stdev(list([float(x["ax"]) for x in chunk])),
        "std_ay": stdev(list([float(x["ay"]) for x in chunk])),
        "std_az": stdev(list([float(x["az"]) for x in chunk])),
        "std_gx": stdev(list([float(x["gx"]) for x in chunk])),
        "std_gy": stdev(list([float(x["gy"]) for x in chunk])),
        "std_gz": stdev(list([float(x["gz"]) for x in chunk])),
        "max_ax": max(list([float(x["ax"]) for x in chunk])),
        "max_ay": max(list([float(x["ay"]) for x in chunk])),
        "max_az": max(list([float(x["az"]) for x in chunk])),
        "max_gx": max(list([float(x["gx"]) for x in chunk])),
        "max_gy": max(list([float(x["gy"]) for x in chunk])),
        "max_gz": max(list([float(x["gz"]) for x in chunk])),
        "min_ax": min(list([float(x["ax"]) for x in chunk])),
        "min_ay": min(list([float(x["ay"]) for x in chunk])),
        "min_az": min(list([float(x["az"]) for x in chunk])),
        "min_gx": min(list([float(x["gx"]) for x in chunk])),
        "min_gy": min(list([float(x["gy"]) for x in chunk])),
        "min_gz": min(list([float(x["gz"]) for x in chunk])),
        "label": Counter(list([x["label"] for x in chunk])).most_common(1)[0][0]
    }


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

        # split raw data by window_size
        chunk = list(islice(raw_data, window_size * freq))
        while chunk:
            feats = compute_features(chunk)
            pattern = f"{feats['label']} 1:{feats['avg_ax']} 2:{feats['avg_ay']} 3:{feats['avg_az']} 4:{feats['avg_gx']} 5:{feats['avg_gy']} 6:{feats['avg_gz']} 7:{feats['std_ax']} 8:{feats['std_ay']} 9:{feats['std_az']} 10:{feats['std_gx']} 11:{feats['std_gy']} 12:{feats['std_gz']} 13:{feats['max_ax']} 14:{feats['max_ay']} 15:{feats['max_az']} 16:{feats['max_gx']} 17:{feats['max_gy']} 18:{feats['max_gz']} 19:{feats['min_ax']} 20:{feats['min_ay']} 21:{feats['min_az']} 22:{feats['min_gx']} 23:{feats['min_gy']} 24:{feats['min_gz']}\n"
            if np.random.uniform() < percent:
                # write to training set
                train_file.write(pattern)
            else:
                # write to test set
                test_file.write(pattern)
            chunk = list(islice(raw_data, window_size * freq))

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
        train_cnt = 0
        test_cnt = 0
        while chunk:
            if np.random.uniform() < percent:
                # write to training set
                file_path = os.path.join(train_dir, f"{train_cnt}.csv")
                train_cnt += 1
            else:
                # write to test set
                file_path = os.path.join(test_dir, f"{test_cnt}.csv")
                test_cnt += 1

            with open(file_path, "w+") as f:
                w = csv.DictWriter(f, chunk[0].keys())
                w.writeheader()
                w.writerows(chunk)

            chunk = list(islice(raw_data, window_size * freq))


if __name__ == "__main__":
    main()
