import csv
from itertools import islice
import os
import click
import numpy as np
from data_loader import load_raw_data


@click.command()
@click.argument("src", type=click.Path(dir_okay=False, exists=True))
@click.option("-w", "--window_size", type=int, default=10, show_default=True, help="Size of the window to apply.")
@click.option("-f", "--freq", type=int, default=100, show_default=True, help="Sampling frequency in Hz.")
@click.option("-d", "--dst", type=click.Path(file_okay=False), default="data", show_default=True, help="Folder where to save generated data. This will contain a sub-folder named as the window_size.")
@click.option("-s", "--percent", type=click.FloatRange(0.0, 1.0), default=0.80, show_default=True, help="Percentage of data put into the training set compared to that in the test set.")
def main(src, window_size, freq, dst, percent):
    """Loads the database .csv file and split it indo training and
    testing sets.
    """
    dst = os.path.join(dst, str(window_size))

    # load raw data
    raw_data = iter(load_raw_data(src))

    # ensure destination directories
    train_dir = os.path.join(dst, "train")
    test_dir = os.path.join(dst, "test")
    os.makedirs(train_dir, exist_ok=True)
    os.makedirs(test_dir, exist_ok=True)

    print(
        f"Splitting database '{src}' in '{train_dir}' and '{test_dir}' with {percent*100}% split.")
    print("This may take some time...")

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
