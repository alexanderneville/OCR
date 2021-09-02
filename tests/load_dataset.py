import json
import matplotlib.pyplot as plt
import numpy as np

def visualise_inputs(characters):

    for i in range(10):
        plt.subplot(2,5,i+1)
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))

    plt.show()

with open("data/numbers.json") as f:
    config = json.load(f)

print("the type of the config var is: ", type(config))
print("the type of the characters var is: ", type(config['characters']))
print("the type of the characters var[0] is: ", type(config['characters'][0]))

characters_to_display = []

for i in range(10):
    characters_to_display.append(config["characters"][i]["pixels"][0])

visualise_inputs(characters_to_display)

# with open("letters.json", "w") as f:
#     json.dump(config, f, indent=4)
