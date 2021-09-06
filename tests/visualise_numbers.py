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

    with open("datasets/numbers.json") as f:
        data = json.load(f)

    characters_to_display = []

    for i in range(10):
        characters_to_display.append(data["characters"][i]["pixels"][0])

    visualise_inputs(characters_to_display)

if __name__ == "__main__":
    main()
