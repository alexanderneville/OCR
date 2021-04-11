
import numpy as np

from fc_dense_layer import FC_Dense_Layer
from activation_layer import Activation_Layer

class Network(object):

    """This class is designed to represent an entire neural network.

    A network has a list of layers, which is filled with instantiated layer objects."""

    def __init__(self):

        # When a network is instantiated, its list of layers is set to blank.
        self._layers = []
        # For debugging purposes keep track
        self._cost = 0

    def add_layer(self, new_layer):

        """Add a layer to the network."""

        self._layers.append(new_layer)

    def predict(self, input_data):


        """Use the current network to predict the label for the input data."""

        # Create a blank array to store the output data.
        outputs = []

        # Iterate through each piece of data.
        for i in range(len(input_data)):

            current_sample = input_data[i]

            # Pass the data through every layer in the network.
            for layer in self._layers:

                current_sample = layer.feed_forward(current_sample)

            # Once the data has been passed all the way through the network, add it to the list of output data.
            outputs.append(current_sample)

        # Finally return the predicted values.
        return outputs


    def train(self, input_data, correct_outputs):


        for i in range(len(input_data)):

            current_sample = input_data[i]

            for layer in self._layers:

                current_sample = layer.feed_forward(current_sample)
