#!/usr/bin/env python3
import abc


class Base_Layer(abc.ABC):
    """
    This class is the basis of a polymorphic representation of neural network layers.
    Classes representing FC layers and activation layers inherit from this class.
    """

    @abc.abstractmethod
    def feed_forward(self, input_data):
        pass

    @abc.abstractmethod
    def propagate_backward(self, error, learning_rate: float):
        pass
