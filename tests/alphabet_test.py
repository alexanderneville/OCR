import os, sys
import numpy as np
import matplotlib.pyplot as plt
import json

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
import pipeline
import activation_calculator as ac
from dataset_functions import *

def process_alphabet():

    image = pipeline.Pipeline()
    image.load_file("input/alphabet.png")
    image.scan_image()
    image.generate_dataset("output/alphabet_dataset.txt", "output/alphabet_sample.txt", "output/alphabet_info.json")
    del(image)

def display_alphabet(characters):

    for i in range(len(characters)):
        plt.subplot(6,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()

def display_one_character(character):
    plt.subplot(1,1,1)
    plt.imshow(character, cmap=plt.get_cmap('gray'))
    plt.show()

def simulate_alphabet_labeling():

    # assume that the user looked through the samples, discarding some of them.

    with open ("./output/alphabet_info.json", "r") as info:
        data = json.load(info)

    for i in range(len(data["characters"])):
        data["characters"][i]["label"] = "abcdefghijklmnopqrstuvwxyz"[i]

    # del(data["characters"][1])
    # del(data["characters"][8])
    # del(data["characters"][14])
    # del(data["characters"][19])

    with open("./output/alphabet_info.json", "w") as f:
        json.dump(data, f, indent=4)

def test_nn_with_alphabet():

    process_alphabet()
    simulate_alphabet_labeling()
    data = get_info("./output/alphabet_info.json")
    training_dataset, sample_dataset = get_datasets(data, "./output/alphabet_dataset.txt", "./output/alphabet_sample.txt")
    display_alphabet(sample_dataset)

    # prepare the training data
    labels = [character["label"] for character in data["characters"]]
    network = nn.Network([32*32, 75, 75, len(labels)], labels)
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(10):
            dataset_outputs.append(labels[i])
    y_correct = network.calc_y_activations(network.lables, dataset_outputs)

    # train the network
    network.train(training_dataset, y_correct, 5000)
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.lables, y_predicted)
    print(outputs)

    # export the model
    print("\nSaving model to file.")
    network.export_layout("./models/alphabet_model.json")
    del(network)
    print("loading model.")

    # import the model
    network = nn.Network.import_layout("./models/alphabet_model.json")
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.lables, y_predicted)
    print(outputs)

if __name__ == "__main__":
    test_nn_with_alphabet()
