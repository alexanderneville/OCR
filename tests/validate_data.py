import json

with open("data/numbers.json") as f:
    data = json.load(f)

for i in range(len(data["characters"])):
    for j in range(len(data["characters"][i])):
        print(len(data["characters"][i]["pixels"][j]))
        print(len(data["characters"][i]["pixels"][j][0]))
