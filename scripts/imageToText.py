from PIL import Image
import sys
import json

if len(sys.argv) != 4:
    print("Usage: imageToText.py [image.png] [definition.json] [text.txt]")
    exit(1)

image = Image.open(sys.argv[1])
definition_file = open(sys.argv[2], "r")
text_file = open(sys.argv[3], "w+")

definition_json = "".join(definition_file.readlines())
definition = json.loads(definition_json)

output = []

image_data = list(image.getdata())

line_counter = 0
width, height = image.size
for data in image_data:
    hex_string = "".join("{:02X}".format(c) for c in data[:3])

    if hex_string in definition:
        output.append(str(definition[hex_string]))

for x in range(height):
    output.insert((x+1) * width + x, "\n")

text_file.write("".join(output))
definition_file.close()
text_file.close()




