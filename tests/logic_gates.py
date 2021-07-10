import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")

import neural_network as nn
import numpy as np

def calc_output(activations):

    output = []

    for i in range(len(activations)):

        output.append(round(activations[i][0][0]))

    return output

print("""

+--------+
| inputs |
+--------+
|   00   |
|   01   |
|   10   |
|   11   |
+--------+

""")

# Instantiate the network
network = nn.Network([2,4,4,1])

# The input data
input_data = np.array([[[0,0]], [[0,1]], [[1,0]], [[1,1]]])
# Labels for the input data.
logical_and = np.array([[[0]], [[0]], [[0]], [[1]]])
logical_or = np.array([[[0]], [[1]], [[1]], [[1]]])
logical_nand = np.array([[[1]], [[1]], [[1]], [[0]]])
useless_gate = np.array([[[1]], [[1]], [[1]], [[1]]])


print("solving 'and' gate")
network.train(input_data, logical_and, 1000, 0.1)
print(calc_output(network.predict(input_data)))

print("\nsolving 'or' gate")
network.train(input_data, logical_or, 1000, 0.1)
print(calc_output(network.predict(input_data)))

print("\nsolving 'nand' gate")
network.train(input_data, logical_nand, 1000, 0.1)
print(calc_output(network.predict(input_data)))

print("\nsolving useless gate")
network.train(input_data, useless_gate, 1000, 0.1)
print(calc_output(network.predict(input_data)))

print("\n")
