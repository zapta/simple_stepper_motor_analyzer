# Converts screenshot dump from the stepper analyzer to a png.
# Crude and for developement purposes only.
#
# To install PIL package run:
#   pip install Image

import sys
from PIL import Image, ImageDraw
import os
from datetime import datetime


# Read lines from input file
file1 = open('./screenshot.txt', 'r')
lines = file1.readlines()
# lines read so far
l = 0

dateTimeObj = datetime.now()
image_count = 0


image = None

# Returns the filename.
def init_new_file():
    global image_count, image
    image_count = image_count + 1
    image = Image.new(mode="RGB", size=(480, 320), color="red")
    return "%d%02d%02d-%02d%02d%02d-%02d.png" % (dateTimeObj.year, dateTimeObj.month,  dateTimeObj.day, dateTimeObj.hour,
                                                 dateTimeObj.minute, dateTimeObj.second, image_count)


# Set one pixel in the image.

# Parse the 24bit pixel color and set in in the image buffer.
def put_pixel(x, y, color):
    global image

    # r3 = (color >> 11) & 0x1f  # R 5 bits
    # g3 = (color >> 5) & 0x3f  # G 6 bits
    # b2 = color & 0x1f  # B 5 bits

    # r = int(r3 * 255 / 31)
    # g = int(g3 * 255 / 63)
    # b = int(b2 * 255 / 31)

    r = (color >> 16) & 0xff  # R 8 bits
    g = (color >> 8) & 0xff  # G 8 bits
    b = color & 0xff  # B 8 bits

    # Images are kind of dark so making this brighter.
    #k = 1.5
    k = 2.0
    #k = 1.0

    r = min(255, int(r * k))
    g = min(255, int(g * k))
    b = min(255, int(b * k))

    image.putpixel((x, y), (r, g, b, 255))

# Process a line with pixels.


def process_data_line(l, line):
    print(f"Processing input line {l+1}")
    if not line.startswith("#"):
        raise Exception(f"Data lines {l+1} doesn't start with a #.")
    tokens = line[1:].split(',')
    x0 = int(tokens[0])
    y0 = int(tokens[1])
    # n = int(tokens[2])
    color_tokens = tokens[3:]
    x = x0
    for color_token in color_tokens:
        parts = color_token.split(':')
        count = int(parts[0])
        color = int(parts[1], 16)
        for i in range(count):
            put_pixel(x, y0, color)
            x += 1


def process_next_image():
    global l
    # Lookup for next start marker or end of input.
    while True:
        if l >= len(lines):
            return
        if lines[l].startswith("###BEGIN"):
            l += 1
            break
        l += 1

    #image = Image.new(mode="RGB", size=(480, 320), color="red")
    filename = init_new_file()

    # Start marker found, process data lines.
    while True:
        if l >= len(lines):
            raise Exception("###END line is missing")
        line = lines[l].rstrip()
        if line.startswith("###END"):
            break
        process_data_line(l, line)
        l += 1

    hist = image.histogram()
    print()
    for i in range(256):
        print("%02x %5d %5d %5d" % (i, hist[i], hist[256+i], hist[512 + i]))
    print()

    image.save(filename)
    print("Saved image", filename)

# Increment l to the index of data line past the BEGIN line.
while l < len(lines):
    process_next_image()
if image_count < 1:
    print("\nError: no image found")
else:
    print("\nAll done, found %d images.\n" % image_count)
