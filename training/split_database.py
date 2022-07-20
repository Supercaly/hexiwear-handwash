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
    testing sets creating two different .csv files.
    """
    dst = os.path.join(dst, str(window_size))

    # load raw data
    raw_data = iter(load_raw_data(src))

    # open destination files
    os.makedirs(dst, exist_ok=True)
    train_file = open(os.path.join(dst, "train.csv"), "w+")
    test_file = open(os.path.join(dst, "test.csv"), "w+")

    print(
        f"Splitting database '{src}' in training and testing inside folder '{dst}' with {percent*100}% split. This may take some time...")

    # split raw data by window_size
    chunk = list(islice(raw_data, window_size * freq))

    # prepare csv writers
    train_csv = csv.DictWriter(train_file, chunk[0].keys())
    test_csv = csv.DictWriter(test_file, chunk[0].keys())
    train_csv.writeheader()
    test_csv.writeheader()

    # write chunks in training file or test file
    # a chunk has 80% probability of going in the
    # training set adn 20% probability in test set.
    while chunk:
        if np.random.uniform() < percent:
            train_csv.writerows(chunk)
        else:
            test_csv.writerows(chunk)
        chunk = list(islice(raw_data, window_size * freq))

    # close csv files
    train_file.close()
    test_file.close()


if __name__ == "__main__":
    main()
