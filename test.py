
import numpy as np
import neural_network as nn

#training_data = np.array([[[0],[0]], [[0],[1]],[[1],[0]],[[1],[1]]])
#training_data_labels = np.array([[[0]], [[1]], [[1]], [[0]]])
training_data = np.array([[[0,0]], [[0,1]], [[1,0]], [[1,1]]])
training_data_labels = np.array([[[0]], [[1]], [[1]], [[0]]])

network = nn.Network()

network.add_layer(nn.FC_Dense_Layer(2,3))
network.add_layer(nn.Activation_Layer(nn.tanh, nn.tanh_derivative))
network.add_layer(nn.FC_Dense_Layer(3,1))
network.add_layer(nn.Activation_Layer(nn.tanh, nn.tanh_derivative))


print(network.predict(training_data))
network.train(training_data, training_data_labels, 0.1)
print(network.predict(training_data))
