
import numpy as np

def variance(correct, predicted):

    """

    The 'correct' parameter is the vector of true values that the training data is labelled with.
    The 'predicted' parameter is the vector of values produced by the network.

    This function is designed to calculate the variance of the data.

    """

    return np.mean(np.power((correct - predicted), 2))



def variance_derivative(correct, predicted):

    """

    This function calculates the derivative of the variance.
    You can see the resembelance to the expression above.

    """

    return (2 * (correct - predicted)) / correct.size
