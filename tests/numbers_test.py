import os, sys
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image as im
import json

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
import pipeline
import activation_calculator as ac
from dataset_functions import *

def process_numbers():
    image = pipeline.Pipeline()
    image.load_file("input/numbers.png")
    image.scan_image()
    image.generate_dataset("output/numbers_dataset.txt", "output/numbers_sample.txt", "output/numbers_info.json")
    del(image)

def display_numbers(characters):
    for i in range(len(characters)):
        plt.subplot(5,2,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))
    plt.show()

def save_numbers(characters):
    for i in range(len(characters)):
        characters[i] = characters[i].astype(int)
        plt.imsave("./output/numbers/"+str(i)+".png", characters[i], cmap=plt.get_cmap('gray'))

def simulate_number_labeling():
    with open ("./output/numbers_info.json", "r") as info:
        data = json.load(info)
    for i in range(len(data["characters"])):
        data["characters"][i]["label"] = i
    with open("./output/numbers_info.json", "w") as f:
        json.dump(data, f, indent=4)

def invert_colours(samples):
    threshold = np.full((32,32), 255.0)
    for i in range(len(samples)):
        samples[i] = np.subtract(threshold, samples[i])

def test_nn_with_numbers():
    # load data
    process_numbers()
    simulate_number_labeling()
    data = get_info("./output/numbers_info.json")
    training_dataset, sample_dataset = get_datasets(data, "./output/numbers_dataset.txt", "./output/numbers_sample.txt")
    invert_colours(sample_dataset)
    display_numbers(sample_dataset)
    save_numbers(sample_dataset)
    invert_colours(sample_dataset)

    # prepare the training data
    labels = [character["label"] for character in data["characters"]]
    network = nn.Network([32*32, 75, 75, len(list(set(labels)))], list(set(labels)))
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(10):
            dataset_outputs.append(labels[i])
    y_correct = network.calc_y_activations(network.labels, dataset_outputs)
    # train the network
    network.train(training_dataset, y_correct, 500)
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.labels, y_predicted)
    print(outputs)

    # export the model
    print("\nSaving model to file.")
    network.export_layout("./models/numbers_model.json")
    del(network)
    print("loading model.")
    # import the model
    network = nn.Network.import_layout("./models/numbers_model.json")
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.labels, y_predicted)
    print(outputs)


if __name__ == "__main__":
    test_nn_with_numbers()
