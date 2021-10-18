import json
import numpy as np
from . import functions
from . import layers


import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/tests")
import debug

class Network(object):

    """ A representation of a fully connected neural network. """
    def __init__(self, layout: list[int], labels: list[str]) -> None:

        # set fields
        self._layout = layout
        self._labels = labels
        self._layers: list[layers.Base_Layer] = []
        self._cost = 0
        self._activation = "tanh"

        # create layers according to layout parameter
        for x, y in zip(layout[:-1], layout[1:]):
            self._layers.append(layers.FC_Dense_Layer(x, y))
            self._layers.append(layers.Activation_Layer(functions.tanh, functions.tanh_derivative))

    @property
    def labels(self):

        """ Return the list of possible labels for output data. """
        return self._labels

    @property
    def network_parameters(self) -> tuple[list[np.ndarray], list[np.ndarray]]:

        """ Return tuple containing lists of the tunable parameters of the network. """

        weights = []
        biases = []

        for i in self._layers:
            # iterate through layers
            if isinstance(i, layers.FC_Dense_Layer):
                # only fully connected layers have parameters
                current_layer_parameters = i.layer_parameters
                weights.append(current_layer_parameters[0])
                biases.append(current_layer_parameters[1])

        return weights, biases

    def add_layer(self, new_layer: layers.Base_Layer) -> None:

        """Given a layer class, add it to the network."""

        self._layers.append(new_layer)

    @staticmethod
    def calc_y_activations(labels, y_correct):

        """ For each sample, create a vector of activations. """

        # list comprehension to generate empty vector for each output
        activations = [[[0 for node in range(len(labels))]] for sample in range(len(y_correct))]
        for sample in range(len(y_correct)):
            # set the correct index to 1
            activations[sample][0][labels.index(y_correct[sample])] = 1

        return np.array(activations)

    @staticmethod
    def calc_results(labels, y_activations):
        """ For each vector of activations, find the label. """
        outputs = [labels[vector.tolist()[0].index(max(vector[0]))] for vector in y_activations]
        return outputs

    def predict(self, input_data, single_sample: bool = False):

        """ Use the current network configuration to predict the activations for a sample or set of samples. """

        if single_sample:
            output = np.array([input_data.flatten()])
            # pass the sample through all layers of the network
            for layer in self._layers:
                output = layer.feed_forward(output)
        else:
            output = []
            # if the function is passed a list of samples, iterate through them
            for i in range(len(input_data)):
                current_sample = np.array([input_data[i].flatten()])
                # pass the sample through all layers of the network
                for layer in self._layers:
                    current_sample = layer.feed_forward(current_sample)
                output.append(current_sample)

        return output

    def batch_learning(self, batch, correct_outputs, learning_rate) -> None:

        """ Implement mini-bath gradient descent."""

        # obtain network parameters
        weights, biases = self.network_parameters
        # initialise empty matrices
        nabla_weights = [np.zeros(w.shape) for w in weights]
        nabla_biases = [np.zeros(b.shape) for b in biases]

        for sample_num in range(len(batch)):
            # backpropogation for each sample
            current_sample = np.array(batch[sample_num].flatten())
            # run the current sample through the network forwards
            output = self.predict(current_sample, single_sample=True)
            # calculate steepest descent for the example
            self._cost += functions.cost(correct_outputs[sample_num], output)
            nabla_cost = functions.d_cost(correct_outputs[sample_num], output)
            # initialise empty matrices to receive changes to w and b
            delta_weights = [np.zeros(w.shape) for w in weights]
            delta_biases = [np.zeros(b.shape) for b in biases]
            layer_num = -1  # keep track of progress backwards through network.

            for layer in reversed(self._layers):
                # backwards direction through network
                if isinstance(layer, layers.FC_Dense_Layer):
                    # compute the necessary changes to w and b for this sample
                    costs = layer.propagate_backward(nabla_cost, learning_rate, calc_only=True)  # input for next layer
                    delta_weights[layer_num] = costs[1]
                    delta_biases[layer_num] = costs[2]
                    layer_num -= 1
                    nabla_cost = costs[0]
                else:
                    nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)
            # combination of parrallel iteration and list comprehension to store updated steepest descent for w and b
            nabla_weights = [current + delta for current, delta in zip(nabla_weights, delta_weights)]
            nabla_biases = [current + delta for current, delta in zip(nabla_biases, delta_biases)]

        # once n_w and n_b are know for the whole minibatch...
        for layer in self._layers:
            # iterate over layers
            if isinstance(layer, layers.FC_Dense_Layer):
                # if tunable parameters...
                # apply computed gradient descent and learning rate
                layer.update((learning_rate / len(batch)) * nabla_weights.pop(0),
                             (learning_rate / len(batch)) * nabla_biases.pop(0))

    def train(self, input_data, correct_outputs, epochs: int = 1, learning_rate: float = 0.1, batch_size: int = None):

        """ Using given data, feed the  data through the network in both directions. """

        for i in range(epochs):
            # reset the cost for each epoch
            self._cost = 0
            if not batch_size:
                # if stochastic descent is chosen
                for sample_num in range(len(input_data)):
                    # flatten the input image, so it is a column vector
                    current_sample = np.array([input_data[sample_num].flatten()])
                    # obtain the output layer activations for each sample
                    output = self.predict(current_sample, single_sample=True)
                    # calculate the cost for the given sample
                    self._cost += functions.cost(correct_outputs[sample_num], output)
                    # nabla is the mathematical symbol for steepest descent
                    # use cost prime to calculate this vector
                    nabla_cost = functions.d_cost(correct_outputs[sample_num], output)
                    # backpropagation, iterate backwards over layers
                    for layer in reversed(self._layers):
                        if isinstance(layer, layers.FC_Dense_Layer):
                            nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)[0]
                        else:
                            nabla_cost = layer.propagate_backward(nabla_cost, learning_rate)
            else:
                # list comprehension to divide whole dataset into mini-batches
                batches = [input_data[b:(b + batch_size)] for b in range(0, len(input_data), batch_size)]
                for batch_num in range(len(batches)):
                    # apply the batch learning function to the current batch
                    self.batch_learning(batches[batch_num],
                                        correct_outputs[(batch_size * batch_num):(batch_size * batch_num + batch_size)],
                                        learning_rate)

            print(f"Epoch: {i + 1}, Average Cost: {self._cost / len(input_data)}, Remaining: {epochs - (i + 1)}")

    def export_layout(self, path):

        # generate the lists of parameters to export
        weights, biases = self.network_parameters
        list_weights = [weights[i].tolist() for i in range(len(weights))]
        list_biases = [biases[i].tolist() for i in range(len(biases))]
        # setup a dictionary containing the data
        config = {
            "layout": self._layout,
            "labels": self._labels,
            "actication": self._activation,
            "weights": list_weights,
            "biases": list_biases,
        }
        # write to file
        with open(path, "w") as f:
            json.dump(config, f, indent=4)

    @staticmethod
    def import_layout(path):

        with open(path) as f:
            config = json.load(f)
        # instantiate a new network with the right characteristics
        new = Network(config["layout"], config["labels"])
        # set the parameters to the correct values, effectively train instantly
        for layer in new._layers:
            if isinstance(layer, layers.FC_Dense_Layer):
                # activation layers do not have configurable parameters
                layer.set_layer_parameters(np.array(config["weights"].pop(0)), np.array(config["biases"].pop(0)))
        # return the new nn model
        return new