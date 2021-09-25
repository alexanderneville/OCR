import os, sys
import numpy as np
import matplotlib.pyplot as plt
import json

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
import pipeline
import activation_calculator as ac

def process_image():

    image = pipeline.Pipeline()
    image.load_file("input/numbers.png")
    image.scan_image()
    image.generate_dataset("output/numbers_dataset.txt", "output/numbers_sample.txt", "output/numbers_info.json")
    del(image)

def visualise_inputs(characters):

    for i in range(10):
        plt.subplot(2,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()

def convert_type(string):
    return float(string)

def main():

    process_image()

    with open ("./output/numbers_info.json", "r") as info:
        data = json.load(info)

    characters = []
    with open("./output/numbers_dataset.txt", "r") as dataset:
        for i in range(len(data["characters"])):
            current_character = []
            for j in range(32):
                row = dataset.read(8*32)[:-1]
                row_pixels = row.split(",")
                row_pixels = list(map(convert_type, row_pixels))
                current_character.append(row_pixels)

            if (i < len(data["characters"]) - 1):
                dataset.seek((i + 1) * 10 * 8 * 32 * 32)
            characters.append(np.array(current_character))

    # visualise_inputs(characters)
    labels = [i for i in "9876543210"]

    # y_activations = ac.calc_y_activations(labels, [i for i in "0123456789"])
    y_correct = ac.calc_y_activations(labels, [i for i in "0123456789"])
    network = nn.Network([32*32, 75, 75, 10])
    network.train(characters, y_correct, 500)
    y_predicted = network.predict(characters)
    outputs = ac.calc_results(labels, y_predicted)
    print(outputs)

if __name__ == "__main__":
    main()
