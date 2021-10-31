import os, sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/src")

import neural_network as nn
import numpy as np


def calc_output(activations):
    output = []

    for i in range(len(activations)):
        output.append(round(activations[i][0][0]))

    return output


def main():
    # The input data
    input_data = np.array([[[0, 0]], [[0, 1]], [[1, 0]], [[1, 1]]])

    # Instantiate the network
    network = nn.Network([2, 4, 4, 2], ["0", "1"])

    and_outputs = ["0", "0", "0", "1"]
    or_outputs = ["0", "1", "1", "1"]

    # Train on the AND gate
    correct_activations = network.calc_y_activations(network.labels, and_outputs)
    network.train(input_data, correct_activations, 100)
    predicted_activations = network.predict(input_data)
    predicted_outputs = network.calc_results(network.labels, predicted_activations)
    print("AND: ", predicted_outputs)

    # train on the OR gate
    correct_activations = network.calc_y_activations(network.labels, or_outputs)
    network.train(input_data, correct_activations, 200)
    predicted_activations = network.predict(input_data)
    predicted_outputs = network.calc_results(network.labels, predicted_activations)
    print("OR: ", predicted_outputs)


if __name__ == "__main__":
    main()
