import numpy as np

def evaluate(y_predicted, y_dataset, y_values):

    total = 0

    for label, layer_activations in zip(y_dataset, y_predicted):

        index = calc_output(layer_activations)
        message = "pass" if index == label else "fail"
        if index == label: total += 1
        print(f"Correct: {y_values[label]}, Predicted: {y_values[index]}, {message}")

    percentage = (total / len(y_predicted)) * 100
    print(f"\nAccuracy: {percentage}%\n")

def calc_y_activations(y_dataset, y_values):

    y_activations = [[[0 for _ in range(len(y_values))]] for _ in range(len(y_dataset))]

    for i in range(len(y_dataset)):

        y_activations[i][0][y_dataset[i]] = 1

    return np.array(y_activations)

def calc_output(activations) -> int:

    # return the index of the greatest activation
    index: int = 0
    activations = activations.flatten()
    for i in range(len(activations)):
        if activations[i] >= activations[index]:
            index = i

    return index



def validate(data):
    for i in range(len(data["characters"])):
        for j in range(len(data["characters"][i])):
            print(len(data["characters"][i]["pixels"][j]))
            print(len(data["characters"][i]["pixels"][j][0]))
