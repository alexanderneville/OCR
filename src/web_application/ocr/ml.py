from .load_data import *
import os, sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))))
import neural_network as nn


def train_new_model(info_path, dataset_path, model_path):
    """ Given a set of file paths, generate a new machine learning model. """

    # get info and dataset from input files
    info = get_info(info_path)
    training_dataset = get_extended_dataset(info, dataset_path)
    # obtain the list of labels used in the current model
    labels = [character["label"] for character in info["characters"]]
    # instantiate the network, removing any repetition from the labels
    network = nn.Network([32 * 32, 75, 75, len(list(set(labels)))], list(set(labels)))
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(10):
            # there are 10 examples of each character
            dataset_outputs.append(labels[i])
    # train the network and save to file
    y_correct = network.calc_y_activations(network.labels, dataset_outputs)
    network.train(training_dataset, y_correct, 500)
    network.export_layout(model_path)


def use_existing_model(info_path, sample_path, model_path):
    """ Given a set of file paths, apply an existing machine learning model. """

    # get info and dataset from input files
    info = get_info(info_path)
    sample_dataset = get_sample_dataset(info, sample_path)
    # instantiate a new network object
    network = nn.Network.import_layout(model_path)
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.labels, y_predicted)

    # format output
    current_word = 0
    current_line = 1
    output_string = ""
    # format the output string with spaces and new lines
    for i in range(len(outputs)):
        if info["characters"][i]["word_number"] != current_word or info["characters"][i]["line_number"] != current_line:
            if info["characters"][i]["line_number"] != current_line and current_line != 0:
                output_string += "\n"
            elif current_word != 0:
                output_string += " "
            current_word = info["characters"][i]["word_number"]
            current_line = info["characters"][i]["line_number"]
        output_string += outputs[i]

    return output_string
