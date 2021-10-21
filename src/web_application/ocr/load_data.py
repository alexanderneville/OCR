import numpy as np
import json


def convert_type(string):
    return float(string)


def get_info(path):
    # open the input file
    with open(path, "r") as info:
        data = json.load(info)
    return data


def save_info(data, path):
    # save the input file
    with open(path, "w") as f:
        json.dump(data, f, indent=4)


def get_extended_dataset(info, dataset_path):
    """ Using information from the info file, load images from the dataset file. """

    training_dataset = []
    # open with context manager
    with open(dataset_path, "r") as dataset:
        for character_num in range(len(info["characters"])):
            dataset.seek(info["characters"][character_num]["position"] * 10 * 8 * 32 * 32)
            for sample in range(10):
                current_character = []
                for row_string in range(32):
                    row_string = dataset.read(8 * 32)[:-1]
                    row_pixels = row_string.split(",")
                    row_pixels = list(map(convert_type, row_pixels))
                    current_character.append(row_pixels)
                training_dataset.append(np.array(current_character))
    return training_dataset


def get_sample_dataset(info, sample_path):
    """ Using information from the info file, load images from the sample file. """

    sample_dataset = []
    # open with context manager
    with open(sample_path, "r") as dataset:
        for character_num in range(len(info["characters"])):
            dataset.seek(info["characters"][character_num]["position"] * 8 * 32 * 32)
            current_character = []
            for row in range(32):
                row_string = dataset.read(8 * 32)[:-1]
                row_pixels = row_string.split(",")
                row_pixels = list(map(convert_type, row_pixels))
                current_character.append(row_pixels)
            sample_dataset.append(np.array(current_character))
    return sample_dataset


def get_single_character(position, sample_path):
    with open(sample_path, "r") as dataset:
        dataset.seek(position * 8 * 32 * 32)
        current_character = []
        for row in range(32):
            row_string = dataset.read(8 * 32)[:-1]
            row_pixels = row_string.split(",")
            row_pixels = list(map(convert_type, row_pixels))
            current_character.append(row_pixels)

    return current_character
