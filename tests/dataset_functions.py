from matplotlib import pyplot as plt
import numpy as np
import json
import pipeline


def convert_type(string):
    """Convert a string read from file to a floating point value."""

    return float(string)


def get_info(path):
    """Read data from JSON file into a dictionary and return to call site."""

    with open(path, "r") as info:
        data = json.load(info)
    return data


def get_datasets(data, dataset_path, sample_path):
    """Load CSV datasets from the specified paths."""

    training_dataset = []
    with open(dataset_path, "r") as dataset:
        for character_num in range(len(data["characters"])):
            dataset.seek(data["characters"][character_num]["position"] * 10 * 8 * 32 * 32)
            # generate a list of samples for a specific character
            character_samples = []
            for sample in range(10):
                current_character = []
                for row_string in range(32):
                    row_string = dataset.read(8 * 32)[:-1]
                    row_pixels = row_string.split(",")
                    row_pixels = list(map(convert_type, row_pixels))
                    current_character.append(row_pixels)
                character_samples.append(np.array(current_character))
            training_dataset.append(character_samples)

    sample_dataset = []
    with open(sample_path, "r") as dataset:
        for character_num in range(len(data["characters"])):
            dataset.seek(data["characters"][character_num]["position"] * 8 * 32 * 32)
            # in the sample dataset there is no repetition of each character.
            current_character = []
            for row in range(32):
                row_string = dataset.read(8 * 32)[:-1]
                row_pixels = row_string.split(",")
                row_pixels = list(map(convert_type, row_pixels))
                current_character.append(row_pixels)
            sample_dataset.append(np.array(current_character))
    return training_dataset, sample_dataset


def save_numbers(characters):
    """Create a PNG file for each character read from the input image."""

    for i in range(len(characters)):
        characters[i] = characters[i].astype(int)
        plt.imsave("./output/numbers/" + str(i) + ".png", characters[i], cmap=plt.get_cmap('gray'))


def save_alphabet(characters):
    """Create a PNG file for each character read from the input image."""

    for i in range(len(characters)):
        characters[i] = characters[i].astype(int)
        labels = [i for i in "abcdefghijklmnopqrstuvwxyz"]
        plt.imsave("./output/alphabet/" + labels[i] + ".png", characters[i], cmap=plt.get_cmap('gray'))


def invert_colours(samples):
    """Flip B&W images."""

    threshold = np.full((32, 32), 255.0)
    for i in range(len(samples)):
        samples[i] = np.subtract(threshold, samples[i])


def process_file(file):
    """From the specified file name, obtain datasets."""

    image = pipeline.Pipeline()
    image.load_file(f"input/{file}.png")
    image.scan_image()
    image.generate_dataset(f"output/{file}_dataset.txt", f"output/{file}_sample.txt", f"output/{file}_info.json")
    del (image)
