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

    def __init__(self, layout):

        """
        A list is passed in with each element representing how many neurons required in each layer.
        """

        # When a network is instantiated, its list of layers is set to blank.
        self._layers: list[layers.Base_Layer] = []
        # For debugging purposes keep track of the network cost as the network is corrected.
        self._cost = 0

        for x, y in zip(layout[:-1], layout[1:]):

            self._layers.append(layers.FC_Dense_Layer(x, y))
            self._layers.append(layers.Activation_Layer(functions.tanh, functions.tanh_derivative))

    def add_layer(self, new_layer: layers.Base_Layer) -> None:

        """Add a layer to the network."""

        self._layers.append(new_layer)

    def predict(self, input_data: np.array) -> np.array:

        """Use the current network to predict the label for the input data."""

        # Create a blank array to store the output data.
        outputs = []

        # Iterate through each piece of data.
        for i in range(len(input_data)):

            current_sample = np.array([input_data[i].flatten()])


            # Pass the data through every layer in the network.
            for layer in self._layers:
                current_sample = layer.feed_forward(current_sample)

            # Once the data has been passed all the way through the network, add it to the list of output data.
            outputs.append(current_sample)

        # Finally return the predicted values.
        return outputs

    def train(self, input_data, correct_outputs, epochs: int = 1, learning_rate: float = 0.1):

        """input_data[i] is one training example, correct_outputs[i] is the output of the final layer."""

        for i in range(epochs):

            self._cost = 0 # reset cost for each epoch

            for j in range(len(input_data)):

                current_sample = np.array([input_data[j].flatten()])
                # debug.print_data(name="layer by layer", sample=current_sample)

                for layer in self._layers:
                    current_sample = layer.feed_forward(current_sample)
                    # debug.print_data(name="layer by layer", sample=current_sample)

                # debug.print_data(name="output activations", activations=current_sample, true_values=correct_outputs[j])
                self._cost += functions.cost(correct_outputs[j], current_sample)
                delta_error = functions.d_cost(correct_outputs[j], current_sample)

                for layer in reversed(self._layers):
                    delta_error = layer.propagate_backward(delta_error, learning_rate)

            print(f"Epoch: {i+1}, Average Cost: {self._cost/len(input_data)}, Remaining: {epochs - (i+1)}")
