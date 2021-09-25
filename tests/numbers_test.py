import os, sys
import numpy as np
import matplotlib.pyplot as plt
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
        plt.subplot(2,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()

def simulate_number_labeling():

    # assume that the user looked through the samples and labelled them 02346789, deleting 1 and 5 for some reason

    with open ("./output/numbers_info.json", "r") as info:
        data = json.load(info)

    for i in range(len(data["characters"])):
        data["characters"][i]["label"] = i

    del(data["characters"][1])
    del(data["characters"][4])

    with open("./output/numbers_info.json", "w") as f:
        json.dump(data, f, indent=4)

def test_nn_with_numbers():

    # load data
    process_numbers()
    simulate_number_labeling()
    data = get_info("./output/numbers_info.json")
    training_dataset, sample_dataset = get_datasets(data, "./output/numbers_dataset.txt", "./output/numbers_sample.txt")
    display_numbers(sample_dataset)

    # prepare the training data
    labels = [character["label"] for character in data["characters"]]
    network = nn.Network([32*32, 75, 75, len(labels)], labels)
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(10):
            dataset_outputs.append(labels[i])
    y_correct = network.calc_y_activations(network.lables, dataset_outputs)

    # train the network
    network.train(training_dataset, y_correct, 500)
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.lables, y_predicted)
    print(outputs)

    # export the model
    print("\nSaving model to file.")
    network.export_layout("./models/numbers_model.json")
    del(network)
    print("loading model.")

    # import the model
    network = nn.Network.import_layout("./models/numbers_model.json")
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.lables, y_predicted)
    print(outputs)


if __name__ == "__main__":
    test_nn_with_numbers()
