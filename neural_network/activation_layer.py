
import numpy as np

from .layer import Layer

class Activation_Layer(Layer):

    """

    The activation layer inherits from the 'Layer' class.
    One of these layers sits between each dense layer in the network.
    The activation layer has as many inputs as the previous layer has outputs.
    Various activation functions might be used:

    1. sigmoid
    2. rlu
    3. tanh

    """
    def __init__(self, function, function_derivative):

        """

        Assign the choosen activation function to this layer.
        Both the function and the derivative of the function are passed in.

        """

        self.chosen_function = function
        self.chosen_function_derivative = function_derivative


    def feed_forward(self, input_data):

        """

        Like other layers this function is called as data is fed through the network.
        Therefore the forward function takes in a row vector and returns a vector of the same shape.

        """

        # record the input for back propogation.
        self._input_data = input_data

        # use the activation function on the input.
        return self.chosen_function(self._input_data)



    def propagate_backward(self, layer_error, learning_rate):

        """

        This layer has no trainable parameters so no adjustments to the layer have to be made.
        This function simply returns dE/dX given dE/dY.
        The learning rate has to be included so that all layers have a common interface even though it is not needed in activation layers.

        """

        return (self.chosen_function_derivative(self._input_data) * layer_error)
