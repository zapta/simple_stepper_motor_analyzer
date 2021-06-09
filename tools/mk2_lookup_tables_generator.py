# A python program to generate the gpio tables in display/lookup_tables_mk2.cpp.
#
# Usage: run with no arguments, paste the output to display/lookup_tables_mk2.cc,
# and reformat the file.
#
# python lookup_tables_generator_mk2.py
#

import sys
# from PIL import Image, ImageDraw
import os
# from datetime import datetime

# In addition to the 16 bit parallel data, we also
# want to reset the WR pin.
WR_PIN = 1

# Maps the 16 data bit index to pin.
DATA_PINS = [
    28, 3, 6, 7,  # D0 - D3
    8, 9, 22, 10,  # D4 - D7
    11, 21, 12, 20,  # D8 - D11
    13, 19, 14, 18  # D12 - D15
]


def printf(format, *args):
    sys.stdout.write(format % args)


# Maps a color value from src_bits to dst_bits.
#
# int src_val: Channel value in src representation.
# int src_bits: Number of bits in src representation
# int dst_bits: Number of bits in destination representation.
#
# Returns an int with value is destination representation.
def resize_color_channel(src_val, src_bits, dst_bits):
    max_src = ((1 << src_bits) - 1)
    max_dst = ((1 << dst_bits) - 1)
    ratio = float(src_val) / max_src
    result = round(ratio * max_dst)
    assert result >= 0
    assert result <= max_dst
    return result

  # Map a 8 bit RGB332 color to a 16 bit RGB565 color value.


def color8_to_color16(c8):
    assert c8 >= 0
    assert c8 <= 255
    assert type(c8) is int

    r3 = (c8 >> 5) & 0x7
    g3 = (c8 >> 2) & 0x7
    b2 = c8 & 0x3

    r5 = resize_color_channel(r3, 3, 5)
    g6 = resize_color_channel(g3, 3, 6)
    b5 = resize_color_channel(b2, 2, 5)

    rgb565 = r5 << 11 | g6 << 5 | b5
    assert rgb565 >= 0
    assert rgb565 < (1 << 16)
    assert type(rgb565) is int
    return rgb565


# Given a 16 bits data value and a port, return the gpio
# set and clr masks to output this value without affecting
# the other gpio pins. The WR_PIN is always included in the
# two masks to generate the WR pulse.
def uint16_value_to_gpio_masks(uint16_value):
    assert uint16_value >= 0
    assert uint16_value < 1 << 16
    assert type(uint16_value) is int

    # We always clear the WR pin, to generate the WR pulse.
    #
    # NOTE: also setting the WR pin using the set table doesn't
    # work because it doesn't provide sufficient hold time for
    # the data before the WR low-to-high transition.
    #
    gpio_set_mask = 0
    gpio_clr_mask = (1 << WR_PIN)
    for bit in range(0, 16):
        pin_index = DATA_PINS[bit]
        pin_mask = (1 << pin_index)
        if ((uint16_value & (1 << bit)) != 0):
            gpio_set_mask |= pin_mask
        else:
            gpio_clr_mask |= pin_mask
    return [gpio_set_mask, gpio_clr_mask]


# A common method to output the table data.
# int[256] uint_values: 16 bits values to encode.
# int reg_index: 0->gpio_set, 1->gpio_clr
def generate_table(uint16_values, table_name, reg_index):
    printf("\n\nconst uint32_t %s[] = {\n", table_name)
    for i in range(0, 256):
        gpio_masks = uint16_value_to_gpio_masks(uint16_values[i])
        printf("0x%08x,", gpio_masks[reg_index])
        if (i % 4 == 3):
            printf("  // 0x%02x - 0x%02x\n", i - 3, i)
    print("};\n")


def main():
    # Print pin map
    print("// PIN MAP:")
    printf("//   WR    %2d\n", WR_PIN)
    print("//")
    for i in range(0, 16):
        # for (int i = 0; i < 16; i++) {
        printf("//   D%-3d  %2d\n", i, DATA_PINS[i])

    direct_values = []
    color16_values = []
    for i in range(0, 256):
        direct_values.append(i)
        color16_values.append(color8_to_color16(i))
    # Direct output. Byte value is preserved as a 16 bit value.
    generate_table(direct_values, "gpio_direct_set_table", 0)
    generate_table(direct_values, "gpio_direct_clr_table", 1)
    # Color mapping. Color8 values are mapped to color16 values.
    generate_table(color16_values, "gpio_color_set_table", 0)
    generate_table(color16_values, "gpio_color_clr_table", 1)


# Start here.
main()
