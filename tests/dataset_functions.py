import numpy as np

def evaluate(predictions, labels) -> None:

    total = 0

    for label, layer_activations in zip(labels, predictions):

        value = calc_output(layer_activations)
        message = "pass" if value == label else "fail"
        if value == label: total += 1
        print(f"Correct: {label}, Predicted: {value}, {message}")

    percentage = (total / len(predictions)) * 100
    print(f"\nAccuracy: {percentage}%\n")

def calc_y_activations(y_dataset):

    y_activations = [[[0 for _ in range(10)]] for _ in range(len(y_dataset))]

    for i in range(len(y_dataset)):

        y_activations[i][0][y_dataset[i]] = 1

    return np.array(y_activations)

def calc_output(activations) -> int:

    index: int = 0
    activations = activations.flatten()
    for i in range(len(activations)):
        if activations[i] >= activations[index]:
            index = i

    return index
