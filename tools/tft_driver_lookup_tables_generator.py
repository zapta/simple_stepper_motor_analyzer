# A python program to generate the lookup table used by the tft driver to 
# map 8bits colors to 16bits colors to send to the TFT.
#
# Usage: run with no arguments, paste the output to display/tft_driver_lookup_tables.cc,
# and reformat the file.
#
# python tft_driver_lookup_tables_generator.py
#

import sys
import os


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


def main():
    printf("\n\nconst uint16_t color8_to_color16_table[] = {\n")
    for i in range(0, 256):
        color16 = color8_to_color16(i)
        printf("0x%04x,", color16)
        if (i % 4 == 3):
            printf("  // 0x%02x - 0x%02x\n", i - 3, i)
    print("};\n")


# Start here.
main()
