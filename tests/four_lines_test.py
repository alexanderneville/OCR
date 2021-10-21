import os, sys
import numpy as np
import matplotlib.pyplot as plt
import json
from pprint import pprint
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
import pipeline
from dataset_functions import *

def display_four_lines(characters):

    for i in range(26):
        plt.subplot(6,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()

def display_one_character(character):
    plt.subplot(1,1,1)
    plt.imshow(character, cmap=plt.get_cmap('gray'))
    plt.show()

def simulate_four_lines_labeling(file):

    with open(f"./output/{file}_info.json", "r") as info:
        data = json.load(info)

    for i in range(len(data["characters"])):
        data["characters"][i]["label"] = "thisissometestdatasecondlineoftestdatamoretestdataonthislinelastlineoftestdata"[i]

    with open(f"./output/{file}_info.json", "w") as f:
        json.dump(data, f, indent=4)

    with open(f"./output/{file}_info_bak.json", "w") as f:
        json.dump(data, f, indent=4)

def test_nn_with_four_lines():

    process_file("four_lines")
    data = get_info("./output/four_lines_info.json")
    training_dataset, sample_dataset = get_datasets(data, "./output/four_lines_dataset.txt",
                                                    "./output/four_lines_sample.txt")

    # display_four_lines(sample_dataset)
    network = nn.Network.import_layout("./models/four_lines_model.json")
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.labels, y_predicted)
    total_correct = 0
    correct = get_info("./output/four_lines_info_bak.json")
    for i in range(len(outputs)):
        print(outputs[i], correct["characters"][i]["label"])
        if outputs[i] == correct["characters"][i]["label"]:
            total_correct += 1
    percentage = (total_correct/len(outputs)) * 100
    print("Accuracy: ", percentage, "%")

def train_nn_with_four_lines():

    process_file("four_lines")
    simulate_four_lines_labeling("four_lines")
    data = get_info("./output/four_lines_info.json")
    training_dataset, sample_dataset = get_datasets(data, "./output/four_lines_dataset.txt",
                                                    "./output/four_lines_sample.txt")

    # display_four_lines(sample_dataset)

    labels = [character["label"] for character in data["characters"]]
    network = nn.Network([32*32, 50, 50, len(list(set(labels)))], list(set(labels)))
    dataset_outputs = []
    for i in range(len(labels)):
        # for _ in range(10):
        dataset_outputs.append(labels[i])

    y_correct = network.calc_y_activations(network.labels, dataset_outputs)

    network.train(sample_dataset, y_correct, 1000)
    network.export_layout("./models/four_lines_model.json")
    del(network)


if __name__ == "__main__":
    train_nn_with_four_lines()
    test_nn_with_four_lines()
