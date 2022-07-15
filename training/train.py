import collections
import csv
from itertools import islice
import statistics

CHUNK_SIZE = 1000

def load_raw_data(file_path):
    """
    Loads raw data from a given .csv file

    Parameter file_path: the path to the .csv file containing the data

    This function reads the data from the given .csv file and yields the rows one
    at the time creating a generator.
    """
    with open(file_path, 'r') as f:
        rows = csv.reader(f)
        next(rows, None)

        for row in rows:
            yield {
                'ax': float(row[0]),
                'ay': float(row[1]),
                'az': float(row[2]),
                'gx': float(row[3]),
                'gy': float(row[4]),
                'gz': float(row[5]),
                'label': int(row[6]),
            }

def compute_features(raw):
    """
    Computes the features from a given raw data

    Parameter raw: a generator of raw data

    This function takes a generator of raw data and computes the features 
    on them returning another generator. The raw data is splitted in N chunks 
    each of CHUNK_SIZE size, the features are computed and are yielded.
    """
    raw_iter = iter(raw)
    chunk = list(islice(raw_iter, CHUNK_SIZE))
    while chunk:
        yield {
            'avg_ax': statistics.mean([c['ax'] for c in chunk]),
            'avg_ay': statistics.mean([c['ay'] for c in chunk]),
            'avg_az': statistics.mean([c['az'] for c in chunk]),
            'avg_gx': statistics.mean([c['gx'] for c in chunk]),
            'avg_gy': statistics.mean([c['gy'] for c in chunk]),
            'avg_gz': statistics.mean([c['gz'] for c in chunk]),
            'std_ax': statistics.mean([c['ax'] for c in chunk]),
            'std_ay': statistics.mean([c['ay'] for c in chunk]),
            'std_az': statistics.mean([c['az'] for c in chunk]),
            'std_gx': statistics.mean([c['gx'] for c in chunk]),
            'std_gy': statistics.mean([c['gy'] for c in chunk]),
            'std_gz': statistics.mean([c['gz'] for c in chunk]),
            'max_ax': statistics.mean([c['ax'] for c in chunk]),
            'max_ay': statistics.mean([c['ay'] for c in chunk]),
            'max_az': statistics.mean([c['az'] for c in chunk]),
            'max_gx': statistics.mean([c['gx'] for c in chunk]),
            'max_gy': statistics.mean([c['gy'] for c in chunk]),
            'max_gz': statistics.mean([c['gz'] for c in chunk]),
            'min_ax': statistics.mean([c['ax'] for c in chunk]),
            'min_ay': statistics.mean([c['ay'] for c in chunk]),
            'min_az': statistics.mean([c['az'] for c in chunk]),
            'min_gx': statistics.mean([c['gx'] for c in chunk]),
            'min_gy': statistics.mean([c['gy'] for c in chunk]),
            'min_gz': statistics.mean([c['gz'] for c in chunk]),
            'expected_label': collections.Counter([c['label'] for c in chunk]).most_common(1)[0][0]
        }
        chunk = list(islice(raw_iter, CHUNK_SIZE))

def save_features(file_path, features):
    """
    Save the computed features to a .csv file

    Parameter file_path: the path where to save the features
    Parameter features: a generator containing the features to save
    """
    with open(file_path, "w+") as f:
        first = next(features, None)
        writer = csv.DictWriter(f, first.keys())
        writer.writeheader()
        writer.writerow(first)
        writer.writerows(iter(features))

if __name__ == "__main__":
    file_path = 'all_db.csv'
    feature_path = 'all_db_feat.csv'


    print(f"-- Load raw data from file '{file_path}'")
    raw_data = load_raw_data(file_path)
    print("-- Computing features")
    features = compute_features(raw_data)
    print("-- Computing features")
    save_features(feature_path, features)
