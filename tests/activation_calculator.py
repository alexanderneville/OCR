import numpy as np

def calc_y_activations(labels, y_correct):
    """for each sample, create a set of activations."""
    activations = [[[0 for node in range(len(labels))]] for sample in range(len(y_correct))]
    for sample in range(len(y_correct)):
        activations[sample][0][labels.index(y_correct[sample])] = 1

    return np.array(activations)


def calc_results(labels, y_activations):
    """for each set of activations, find the value it corresponds to"""
    outputs = [labels[vector.tolist()[0].index(max(vector[0]))] for vector in y_activations]
    return outputs
