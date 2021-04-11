
# We will use numpy to make the matrices that hold the weights and biases.

import numpy as np

# The base class is needed for the dense layer to inherit from.

from layer import Layer


class FC_Dense_Layer(Layer):


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
        # The number of rows is equal to the number of outputs, with each row having a column for each input.
        self._weights = np.random.randn(num_outputs, num_inputs)

        # That the output (before bias is applied) will have many columns, but just one row.
        # So create a matrix that has one column in every row as opposed to a 1D array.
        self._biases = np.random.rand(num_outputs, 1)

    def feed_forward(self, input_data):

        """

        Given a vector containing all of the outputs of the previous layer, multiply by all of the weights leading to this layer.
        'inputs' is a matrix with one column and many rows.
        The shape is therefore == (num_inputs, 1)

        """

        self._input_data = input_data
        # In order to multiply each input with its respective weight, use np.dot
        # The weights are passed first so the the matrix multiplicaion is valid.
        output = np.dot(self._weights, self._input_data)

        # two numpy arrays can be added like this (short for np.add())
        output += self._biases

        # finally return the output which resembles the input (a single column matrix)
        return output

    def propogate_backward(self, error, learning_rate):

        """

        This function takes in the error with respect to the output of the layer.
        It calculates the necessary changes to the transposed weights and biases.
        Finally it returns the derivative of the network error with respect to the input.
        This value is passed to the previous layer and so on.

        """

        pass
