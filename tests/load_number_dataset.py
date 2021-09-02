import json
import matplotlib.pyplot as plt
import numpy as np

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn

from dataset_functions import *

def visualise_inputs(characters):

    for i in range(10):
        plt.subplot(2,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()


def main():

    with open("data/numbers.json") as f:
        data = json.load(f)

#     with open("letters.json", "w") as f:
#         json.dump(data, f, indent=4)
# 
#     print("the type of the config var is: ", type(data))
#     print("the type of the characters var is: ", type(data['characters']))
#     print("the type of the characters var[0] is: ", type(data['characters'][0]))
# 
#     characters_to_display = []
# 
#     for i in range(10):
#         characters_to_display.append(data["characters"][i]["pixels"][0])
# 
#     visualise_inputs(characters_to_display)

    # print(len(data["characters"][0]["pixels"][0]))
    # print(len(data["characters"][0]["pixels"][0][0]))

    y_values = [i for i in range(10)]

    x_dataset = []
    y_dataset = []

    for i in range(len(data["characters"])):
        for j in range(len(data["characters"][i]["pixels"])):
            x_dataset.append(np.array(data["characters"][i]["pixels"][j]))
            y_dataset.append(i)

    y_activations = calc_y_activations(y_dataset, y_values)

    network = nn.Network([32*32, 75, 75, 10])
    network.train(x_dataset, y_activations, 500, 0.1)
    y_predicted = network.predict(x_dataset)
    evaluate(y_predicted, y_dataset, y_values)

    network.export_layout("data/numbers_model.json")
    new_network = nn.Network.import_layout("data/numbers_model.json")

    y_predicted = new_network.predict(x_dataset)
    evaluate(y_predicted, y_dataset, y_values)



if __name__ == "__main__":
    main()
