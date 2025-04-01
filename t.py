import json

# File containing commands
commands_file = "commands.txt"

# Initialize car starting position and direction
x, y = 0, 0
direction = 0  # 0: Up, 90: Right, 180: Down, 270: Left
path = [(x, y)]

# Function to update direction
def update_direction(current_dir, turn):
    if turn == "R":
        return (current_dir + 90) % 360
    elif turn == "L":
        return (current_dir - 90) % 360
    return current_dir

# Process commands
with open(commands_file, "r") as file:
    for line in file:
        command = line.strip()
        if command.startswith("FORWARD"):
            distance = int(command.split()[1])
            if direction == 0:  # Up
                y += distance
            elif direction == 90:  # Right
                x += distance
            elif direction == 180:  # Down
                y -= distance
            elif direction == 270:  # Left
                x -= distance
            path.append((x, y))
        elif command in ("R", "L"):
            direction = update_direction(direction, command)

# Save the path as JSON for the website
with open("path.json", "w") as json_file:
    json.dump(path, json_file)

print("Path data saved to path.json")

