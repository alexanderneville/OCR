import json

with open("output.json") as f:
    config = json.load(f)

with open("modified.json", "w") as f:
    json.dump(config, f, indent=4)
