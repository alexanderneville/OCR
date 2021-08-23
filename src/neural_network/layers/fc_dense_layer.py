
# We will use numpy to make the matrices that hold the weights and biases.

import numpy as np

# The base class is needed for the dense layer to inherit from.

from .base_layer import Base_Layer


class FC_Dense_Layer(Base_Layer):


    def __init__(self, num_inputs, num_outputs):

        """

        Create a more specific hidden layer that is 'fully connected' to the previous layer.
        This class will have a matrix full of weights and another matrix/vector containing all the biases.
        These matrices are stored as instances of self.

        * num_inputs corresponds to the number of outputs (number of perceptons) in the previous layer.
        * num_outputs corresponds to the number of pereptons in this layer.

        Used together these variables can determine how many wheights are needed.

        """

        # Use numpy to generate a matrix full of random values.
        # The number of rows is equal to the number of inputs, with each row having a column for every output neuron.
        self._weights = (np.random.rand(num_inputs, num_outputs) - 0.5)
        self._biases = (np.random.rand(1, num_outputs) - 0.5)

    def feed_forward(self, input_data):

        """

        Given a vector containing all of the outputs of the previous layer, multiply by all of the weights leading to this layer.
        'inputs' is a matrix with one column and many rows.
        The shape is therefore == (num_inputs, 1)

        """

        self._input_data = input_data
        output = np.dot(self._input_data, self._weights)

        # two numpy arrays can be added like this (short for np.add())
        output += self._biases

        # finally return the output which resembles the input (a single column matrix)
        return output

    def propagate_backward(self, layer_error, learning_rate, calc_only: bool = False):

        """

        This function takes in the error with respect to the output of the layer.
        It calculates the necessary changes to the transposed weights and biases.
        Finally it returns the derivative of the network error with respect to the input.
        This value is passed to the previous layer as the 'layer_error' and so on

        """

        input_error = np.dot(layer_error, np.transpose(self._weights))
        weight_error = np.dot(np.transpose(self._input_data), layer_error)

        if not calc_only:

            self._weights -= (learning_rate * weight_error)
            self._biases -= (learning_rate * layer_error)

        return input_error, weight_error, layer_error

    @property
    def layer_parameters(self):
        return self._weights, self._biases

    def set_layer_parameters(self, weights, biases):
        self._weights = weights
        self._biases = biases

    def update(self, d_weights, d_biases):

        """update parameters by given vectors using mini-batch gradient descent"""

        self._weights -= d_weights
        self._biases -= d_biases
