import json
import numpy as np
from . import functions
from . import layers

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/tests")
import debug

class Network(object):

    """
    This class is designed to represent an entire neural network.
    A network has a list of layers, which is filled with instantiated layer objects.
    Each layer is composed of numpy arrays.
    """

    def __init__(self, layout: list[int]) -> None:

        """
        A list is passed in with each element representing how many neurons required in each layer.
        """

        self._layout = layout

        # When a network is instantiated, its list of layers is set to blank.
        self._layers: list[layers.Base_Layer] = []

        # For debugging purposes keep track of the network cost as the network is corrected.
        self._cost = 0

        self._activation = "tanh"

        for x, y in zip(layout[:-1], layout[1:]):

            self._layers.append(layers.FC_Dense_Layer(x, y))
            self._layers.append(layers.Activation_Layer(functions.tanh, functions.tanh_derivative))

    @property
    def network_parameters(self) -> tuple[list[np.ndarray], list[np.ndarray]]:

        """retun tuple containing lists of the tunable parameters of the network."""

        weights = []
        biases = []

        for i in self._layers:

            if isinstance(i, layers.FC_Dense_Layer):

                current_layer_parameters = i.layer_parameters
                weights.append(current_layer_parameters[0])
                biases.append(current_layer_parameters[1])

        return weights, biases

    def add_layer(self, new_layer: layers.Base_Layer) -> None:

        """Add a layer to the network."""

        self._layers.append(new_layer)

    def predict(self, input_data, single_sample: bool = False):

        """Use the current network to predict the label for the input data."""

        if single_sample:

            output = np.array([input_data.flatten()])
            for layer in self._layers:
                output = layer.feed_forward(output)

        else:

            output = []
            # Iterate through each piece of data.
            for i in range(len(input_data)):

                current_sample = np.array([input_data[i].flatten()])

                # Pass the data through every layer in the network.
                for layer in self._layers:
                    current_sample = layer.feed_forward(current_sample)

                # Once the data has been passed all the way through the network, add it to the list of output data.
                output.append(current_sample)

        return output


    def batch_learning(self, batch, correct_outputs, learning_rate) -> None:

        """perform gradient descent for a given mini-batch of examples"""

        weights, biases = self.network_parameters
        nabla_weights = [np.zeros(w.shape) for w in weights]
        nabla_biases = [np.zeros(b.shape) for b in biases]

        # back propogation for each example
        for sample_num in range(len(batch)):

            current_sample = np.array(batch[sample_num].flatten())
            output = self.predict(current_sample, single_sample=True)
            self._cost += functions.cost(correct_outputs[sample_num], output)
            nabla_cost = functions.d_cost(correct_outputs[sample_num], output)

            delta_weights = [np.zeros(w.shape) for w in weights]
            delta_biases = [np.zeros(b.shape) for b in biases]
            layer_num = -1 # keep track of progress backwards through network.

            for layer in reversed(self._layers):

                if isinstance(layer, layers.FC_Dense_Layer):

                    costs = layer.propagate_backward(nabla_cost, learning_rate, calc_only=True) # input for next layer
                    delta_weights[layer_num] = costs[1]
                    delta_biases[layer_num] = costs[2]
                    layer_num -= 1
                    nabla_cost = costs[0]

                else:

                    nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)

            nabla_weights = [current+delta for current, delta in zip(nabla_weights, delta_weights)]
            nabla_biases = [current+delta for current, delta in zip(nabla_biases, delta_biases)]

        for layer in self._layers:
            if isinstance(layer, layers.FC_Dense_Layer):
                layer.update((learning_rate/len(batch))*nabla_weights.pop(0),
                    (learning_rate/len(batch))*nabla_biases.pop(0))


    def train(self, input_data, correct_outputs, epochs: int = 1, learning_rate: float = 0.1, batch_size: int = None):

        """input_data[i] is one training example, correct_outputs[i] is the output of the final layer."""

        for i in range(epochs):

            self._cost = 0 # reset cost for each epoch

            if not batch_size:

                for sample_num in range(len(input_data)):

                    # print(type(input_data[sample_num]))
                    # print(input_data[sample_num].shape)
                    # print(input_data[sample_num].flatten())
                    # print(np.array([input_data[sample_num].flatten()]).shape)

                    current_sample = np.array([input_data[sample_num].flatten()])
                    output = self.predict(current_sample, single_sample=True)

                    # debug.print_data(name="output activations", activations=current_sample, true_values=correct_outputs[j])
                    self._cost += functions.cost(correct_outputs[sample_num], output)
                    # nabla is the mathematical symbol for steepest descent
                    nabla_cost = functions.d_cost(correct_outputs[sample_num], output)
                    for layer in reversed(self._layers):
                        if isinstance(layer, layers.FC_Dense_Layer):
                            nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)[0]
                        else:
                            nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)

            else:

                batches = [input_data[b:(b+batch_size)] for b in range(0, len(input_data), batch_size)]

                for batch_num in range(len(batches)):

                    self.batch_learning(batches[batch_num], 
                        correct_outputs[(batch_size * batch_num):(batch_size * batch_num + batch_size)],
                        learning_rate)
                        
            print(f"Epoch: {i+1}, Average Cost: {self._cost/len(input_data)}, Remaining: {epochs - (i+1)}")

    def export_layout(self, path):

        weights, biases = self.network_parameters
        list_weights = []
        list_biases = []

        for i in range(len(biases)):
            list_biases.append(biases[i].tolist())
            list_weights.append(weights[i].tolist())

        config = {
            "layout": self._layout,
            "actication": self._activation,
            "weights": list_weights,
            "biases": list_biases,
        }

        with open(path, "w") as f:
            json.dump(config, f, indent=4)

    @staticmethod
    def import_layout(path):

        with open(path) as f:
            config = json.load(f)
        new = Network(config["layout"])

        for layer in new._layers:
            if isinstance(layer, layers.FC_Dense_Layer):
                layer.set_layer_parameters(np.array(config["weights"].pop(0)), np.array(config["biases"].pop(0)))                

        return new
