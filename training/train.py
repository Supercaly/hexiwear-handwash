import os
import click
from cnn import generate_cnn
from lstm import generate_lstm


@click.command()
@click.option("-s", "--src", type=click.Path(file_okay=False, exists=True), default="data", help="Path to a folder containing .csv training and testing sets. This path will be joined with the window_size.")
@click.option("-w", "--window_size", type=int, default=10, show_default=True, help="Size of the window to apply.")
@click.option("-f", "--freq", type=int, default=100, show_default=True, help="Sampling frequency in Hz.")
@click.option("--mode", type=click.Choice(["svm", "knn", "cnn", "lstm"], case_sensitive=False), required=True, help="Select the Machine Learning model type to train.")
@click.option("-d", "--split", type=click.IntRange(min=1), default=1, help="Divide the number of batches per epoch by this number.")
def main(src, window_size, freq, mode, split):
    """
    Trains a Machine Learning model from input data.
    """
    src = os.path.join(src, str(window_size))
    train_dir = os.path.join(src, "train")
    test_dir = os.path.join(src, "test")

    match mode:
        case "svm":
            print(f"SVM target model is not supported!")
            exit(0)
        case "knn":
            print(f"ERS-KNN target model is not supported!")
            exit(0)
        case "cnn":
            generate_cnn(train_dir, test_dir, window_size*freq, split)
        case "lstm":
            generate_lstm(train_dir, test_dir, window_size, freq, split)
        case _:
            print(f"unknown target model '{mode}'!")
            exit(1)


if __name__ == "__main__":
    main()

    # feature_path = 'all_db_feat.csv'
    # has_saved_features = isfile(feature_path)

    # if not has_saved_features:
    #     print(f"-- Load raw data from file '{file_path}'")
    #     raw_data = load_raw_data(file_path)
    #     print("-- Computing features")
    #     features = compute_features(raw_data)

    #     print(f"-- Save features to file '{feature_path}'")
    #     save_features(feature_path, features)
    # else:
    #     print(f"-- Load features from file '{feature_path}'")
    #     features = load_features(feature_path)

    # ((x_train, y_train), (x_test, y_test)) = split_dataset(features)
    # print(f"-- Split dataset")
    # print(f"    training set shape: {x_train.shape}")
    # print(f"    test set shape: {x_test.shape}")
