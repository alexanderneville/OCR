from .base_layer import Base_Layer


class IO_Layer(Base_Layer):
    """Make presentation better. No processing performed."""

    def feed_forward(self, input_data):
        return input_data

    def propagate_backward(self, error, learning_rate):
        return error
