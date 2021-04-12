#!/usr/bin/env python3
import abc

class Layer(abc.ABC):

    @abc.abstractmethod
    def __init__(self):

        pass

    @abc.abstractmethod
    def feed_forward(self, input_data):

        pass

    @abc.abstractmethod
    def propagate_backward(self, error, learning_rate):

        pass
