
from . import functions
from . import layers

class Network(object):
    """This class is designed to represent an entire neural network.

    A network has a list of layers, which is filled with instantiated layer objects."""

    def __init__(self, layout):

        """

        A list is passed in with each element representing how many neurons required in each layer.

        """

        # When a network is instantiated, its list of layers is set to blank.
        self._layers = []
        # For debugging purposes keep track of the network cost as we correct the network.
        self._cost = 0

        for x, y in zip(layout[:-1], layout[1:]):

            self._layers.append(layers.FC_Dense_Layer(x, y))
            self._layers.append(layers.Activation_Layer(functions.tanh, functions.tanh_derivative))

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

    def train(self, input_data, correct_outputs, learning_rate):

        """Given some input data and the correct labels for that data, adjust the network to suit the data."""

        for _ in range(1000):

            for i in range(len(input_data)):

                current_sample = input_data[i]

                for layer in self._layers:
                    current_sample = layer.feed_forward(current_sample)

                network_error = functions.d_cost(correct_outputs[i], current_sample)

                for layer in reversed(self._layers):
                    network_error = layer.propagate_backward(network_error, learning_rate)
