
import numpy as np
from .base_layer import Base_Layer

class IO_Layer(Base_Layer):

    """

    This class serves no practical purpose, but it makes the network as a whole look nicer.

    """

    def feed_forward(self, input_data):

        return input_data


    def propagate_backward(self, error, learning_rate):

        return error
