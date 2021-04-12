
import numpy as np
import neural_network as nn


print("""The input data is:
00
01
10
11""")
input_data = np.array([[[0,0]], [[0,1]], [[1,0]], [[1,1]]])
logical_and = np.array([[[0]], [[0]], [[0]], [[1]]])
logical_or = np.array([[[0]], [[1]], [[1]], [[1]]])

# Create the network
network = nn.Network()

network.add_layer(nn.FC_Dense_Layer(2,3))
network.add_layer(nn.Activation_Layer(nn.tanh, nn.tanh_derivative))
network.add_layer(nn.FC_Dense_Layer(3,3))
network.add_layer(nn.Activation_Layer(nn.tanh, nn.tanh_derivative))
network.add_layer(nn.FC_Dense_Layer(3,1))
network.add_layer(nn.Activation_Layer(nn.tanh, nn.tanh_derivative))


network.train(input_data, logical_and, 0.1)
print(network.predict(input_data))
network.train(input_data, logical_or, 0.1)
print(network.predict(input_data))
