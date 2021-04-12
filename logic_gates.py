
import numpy as np
import neural_network as nn


print("""The input data is:

00
01
10
11

""")

# The input data
input_data = np.array([[[0,0]], [[0,1]], [[1,0]], [[1,1]]])

# Labels for the input data.

logical_and = np.array([[[0]], [[0]], [[0]], [[1]]])
logical_or = np.array([[[0]], [[1]], [[1]], [[1]]])
logical_nand = np.array([[[1]], [[1]], [[1]], [[0]]])
useless_gate = np.array([[[1]], [[1]], [[1]], [[1]]])

# Instantiate the network
network = nn.Network([2,3,3,1])

# Add some layers to the network
# An activation layer follows each fully connected layer.
#network.add_layer(nn.layers.FC_Dense_Layer(2,3))
#network.add_layer(nn.layers.Activation_Layer(nn.functions.tanh, nn.functions.tanh_derivative))
#network.add_layer(nn.layers.FC_Dense_Layer(3,3))
#network.add_layer(nn.layers.Activation_Layer(nn.functions.tanh, nn.functions.tanh_derivative))
#network.add_layer(nn.layers.FC_Dense_Layer(3,1))
#network.add_layer(nn.layers.Activation_Layer(nn.functions.tanh, nn.functions.tanh_derivative))

# Use the nework configured above to learn how to process various logic gates.

print("solving 'and' gate")
network.train(input_data, logical_and, 0.1)
print(network.predict(input_data))

print("\nsolving 'or' gate")
network.train(input_data, logical_or, 0.1)
print(network.predict(input_data))


print("\nsolving 'nand' gate")
network.train(input_data, logical_nand, 0.1)
print(network.predict(input_data))


print("\nsolving useless gate")
network.train(input_data, useless_gate, 0.1)
print(network.predict(input_data))
