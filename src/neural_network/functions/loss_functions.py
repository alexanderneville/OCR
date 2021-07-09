
import numpy as np

def cost(correct, predicted):

    """
    return the cost of a single training example for diagnostics

    """

    return np.mean(np.power((correct - predicted), 2))



def d_cost(correct, predicted):

    """

    This function calculates the derivative of the variance.

    """

    return (2 * (correct - predicted)) / correct.size
