from .load_data import *
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))))
import neural_network as nn

def train_new_model(info_path, dataset_path, model_path):

    info = get_info(info_path)
    training_dataset = get_extended_dataset(info, dataset_path)

    labels = [character["label"] for character in info["characters"]]
    network = nn.Network([32*32, 75, 75, len(labels)], labels)
    dataset_outputs = []
    for i in range(len(labels)):
        for _ in range(10):
            dataset_outputs.append(labels[i])

    y_correct = network.calc_y_activations(network.lables, dataset_outputs)

    network.train(training_dataset, y_correct, 500)
    network.export_layout(model_path)

def use_existing_model(info_path, sample_path, model_path):

    info = get_info(info_path)
    sample_dataset = get_sample_dataset(info, sample_path)

    network = nn.Network.import_layout(model_path)
    y_predicted = network.predict(sample_dataset)
    outputs = network.calc_results(network.lables, y_predicted)

    current_word = 0
    current_line = 1
    output_string = ""
    for i in range(len(outputs)):
        if info["characters"][i]["word_number"] != current_word or info["characters"][i]["line_number"] != current_line:
            if current_word != 0:
                output_string += " "
            current_word = info["characters"][i]["word_number"]
            current_line = info["characters"][i]["line_number"]
        output_string += outputs[i]

    return output_string
