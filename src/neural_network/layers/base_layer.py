#!/usr/bin/env python3
import abc

class Base_Layer(abc.ABC):

    """

    This abstract class is used to build a common interface for all layers to use.
    This is helpful, as it makes feeding data through the network much easier.

    """

    @abc.abstractmethod
    def feed_forward(self, input_data):

        pass

    @abc.abstractmethod
    def propagate_backward(self, error, learning_rate):

        pass
