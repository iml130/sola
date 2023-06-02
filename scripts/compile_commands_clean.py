import json
import sys

if __name__ == "__main__":
    if (len(sys.argv) < 3):
        print("Need path to compile_commands.json and at least one excluding path")
        exit(1)

    cleaned = []

    excluding_paths = sys.argv[2:]

    with open(sys.argv[1], mode="r") as file:
        data = json.load(file)

        for entry in data:
            append = True
            for path in excluding_paths:
                append &= path not in entry["file"]

            if append:
                cleaned.append(entry)
    
    with open(sys.argv[1], mode="w") as file:
        json.dump(cleaned, file, indent=0)
