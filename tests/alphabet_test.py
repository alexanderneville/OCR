import os, sys
import numpy as np
import matplotlib.pyplot as plt
import json
from pprint import pprint

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/src")
import neural_network as nn
import pipeline
from dataset_functions import *


def display_alphabet(characters):
    for i in range(26):
        plt.subplot(6, 5, i + 1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()


def display_one_character(character):
    plt.subplot(1, 1, 1)
    plt.imshow(character, cmap=plt.get_cmap('gray'))
    plt.show()


def simulate_alphabet_labeling(file):
    with open(f"./output/{file}_info.json", "r") as info:
        data = json.load(info)

    for i in range(len(data["characters"])):
        data["characters"][i]["label"] = "abcdefghijklmnopqrstuvwxyz"[i]

    with open(f"./output/{file}_info.json", "w") as f:
        json.dump(data, f, indent=4)


def test_nn_with_alphabet():
    # open, segment and output data from the input file
    process_file("alphabet")
    # obtain labels for the training data
    simulate_alphabet_labeling("alphabet")
    # load the data needed to train a new model
    data = get_info("./output/alphabet_info.json")
    extended_dataset, sample_dataset = get_datasets(data, "./output/alphabet_dataset.txt",
                                                    "./output/alphabet_sample.txt")
    invert_colours(sample_dataset)
    save_alphabet(sample_dataset)
    # create new neural network with the appropriate parameters
    labels = [character["label"] for character in data["characters"]]
    # cast the list of labels to a set to remove repetition
    network = nn.Network([32 * 32, 50, 50, len(list(set(labels)))], list(set(labels)))

    # divide the dataset into training and test data
    training_dataset = []
    for character in extended_dataset:
        # take the last 9 samples for each character
        for sample in character[1:]:
            training_dataset.append(sample)
    test_dataset = sample_dataset

    # prepare the labels and activations for the training dataset
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(9):
            dataset_outputs.append(labels[i])
    correct_activations = network.calc_y_activations(network.labels, dataset_outputs)

    # do the training
    network.train(training_dataset, correct_activations, 3000)

    # export / import the model
    network.export_layout("./models/alphabet_model.json")
    del (network)
    network = nn.Network.import_layout("./models/alphabet_model.json")

    # predict and and evaluate test data
    predicted_activations = network.predict(test_dataset)
    predicted_outputs = network.calc_results(network.labels, predicted_activations)

    total_correct = 0
    correct_outputs = "abcdefghijklmnopqrstuvwxyz"
    for i in range(len(predicted_outputs)):
        print(predicted_outputs[i], correct_outputs[i])
        if predicted_outputs[i] == correct_outputs[i]:
            total_correct += 1
    percentage = (total_correct / len(correct_outputs)) * 100
    print("Accuracy: ", percentage, "%")


if __name__ == "__main__":
    test_nn_with_alphabet()
