# Exports a bitmap file containing the level data
# to a bindary format to be imported by c++

import argparse
import cv2
import numpy as np
import struct
import sys


#############################################################
#                                                           #
#       PIXEL VALUE MAP  & HELPER FUNCTIONS/VARIABLES       #
#                                                           #
#############################################################

COLOR_CHANNELS = {
    'RED': 0,
    'GREEN': 1,
    'BLUE': 2
}

# Mesh IDs mapped to mesh names (refer to design document)
MESH_ID = {
    0: 'EMPTY',
    1: 'MAGPIE',
    2: 'GUARD',
    3: 'FLOOR',
    4: 'DOOR',
    5: 'PAINTING',
    6: 'GEM',
    16: 'WALL',
    17: 'WALL_90',
    18: '4_CORNER',
    19: '3_CORNER',
    20: '3_CORNER_90',
    21: '3_CORNER_180',
    22: '3_CORNER_270',
    23: '2_CORNER',
    24: '2_CORNER_90',
    25: '2_CORNER_180',
    26: '2_CORNER_270',
    27: 'PEDESTAL'
}

MESH_MASK = int('11111000', 2)
MESH_OFFSET = 3

MESH_CUSTOMIZATION_MASK = int('00000111', 2)
MESH_CUSTOMIZATION_OFFSET = 0

ROOM_NUMBER_MASK = int('11110000', 2)
ROOM_NUMBER_OFFSET = 4

GUARD_PATH_MASK = int('00001000', 2)
GUARD_PATH_OFFSET = 3

OBJECT_ID_MASK = int('01110000', 2)
OBJECT_ID_OFFSET = 4

POTENTIAL_PLACEHOLDER_MASK = int('10000000', 2)
POTENTIAL_PLACEHOLDER_OFFSET = 7

INTERACTION_FUNC_ID_MASK = int('00001100', 2)
INTERACTION_FUNC_ID_OFFSET = 2

INTERACTION_FLAG_MASK = int('00000011', 2)
INTERACTION_FLAG_OFFSET = 0

def get_mesh_id(red_channel_data):
    return int((red_channel_data & MESH_MASK) >> MESH_OFFSET)

def get_mesh_customization(red_channel_data):
    return (red_channel_data & MESH_CUSTOMIZATION_MASK) >> MESH_CUSTOMIZATION_OFFSET

def is_guard_path(green_channel_data):
    return bool((green_channel_data & GUARD_PATH_MASK) >> GUARD_PATH_OFFSET)

def get_room_number(green_channel_data):
    return int((green_channel_data & ROOM_NUMBER_MASK) >> ROOM_NUMBER_OFFSET)

def is_potential_placeholder(blue_channel_data):
    return bool((blue_channel_data & POTENTIAL_PLACEHOLDER_MASK) >> POTENTIAL_PLACEHOLDER_OFFSET)

def get_object_id(blue_channel_data):
    return int((blue_channel_data & OBJECT_ID_MASK) >> OBJECT_ID_OFFSET)

def get_interaction_func_id(blue_channel_data):
    return int((blue_channel_data & INTERACTION_FUNC_ID_MASK) >> INTERACTION_FUNC_ID_OFFSET)

def get_interaction_flag(blue_channel_data):
    return int((blue_channel_data & INTERACTION_FLAG_MASK) >> INTERACTION_FLAG_OFFSET)


#############################################################
#                                                           #
#                DEFINE PIXEL VALIDATORS                    #
#                                                           #
#############################################################

# Abstract Validator class to be extended by all level validators
class AbstractValidator:

    def check(self, pixel_data):
        """
        This function is supposed to raise an exception when this pixel
        has invalid values
        """
        assert(len(pixel_data) == 3)

# For each pixel attribute we want to check, we need to extend
# the AbstractValidator class and implement it's check() method
# be sure to add the validator to the validator list created
# before looping throught the pixels

class GuardPathValidator(AbstractValidator):

    def check(self, pixel_data):
        super().check(pixel_data)
        # Check if this tile is part of a guard path
        # if it is, then make sure that the mesh is a floor tile
        if (is_guard_path(pixel_data[COLOR_CHANNELS['GREEN']])):
            mesh_id = get_mesh_id(pixel_data[COLOR_CHANNELS['RED']])
            # 0 is the value for a floor tile
            (mesh_id == 0)

#############################################################
#                                                           #
#              RUN VALIDATION AND EXPORT PIXELS             #
#                                                           #
#############################################################

if (__name__ == "__main__"):
    parser = argparse.ArgumentParser()
    parser.add_argument("img_file", help="path to the bitmap being exported", type=str)
    parser.add_argument("out_file", help="path to the directory where the level file will be exported", type=str)
    parser.add_argument("--stride", help="how many cells to skip", type=int, default=1)
    parser.add_argument("--validate", help="should this script validate the pixel data", action="store_true")
    args = parser.parse_args()

    img_file = args.img_file
    img = cv2.imread(img_file, cv2.IMREAD_COLOR)
    img_shape = img.shape

    # Validators to run while exporting data
    validators = [ GuardPathValidator() ]

    # Byte stream to hold output
    output = b''

    # Iterate over all pixels
    for row in range(0, img_shape[0], args.stride):
        for col in range(0, img_shape[1], args.stride):
            
            # By default CV2 is BGR instead of RGB
            pixel = np.flip(img[row, col], axis=0)

            # Run validators on the level encoding
            if (args.validate):
                for validator in validators:
                    validator.check(pixel)

            # Write this pixel to a binary file
            output += struct.pack('B', pixel[0])
            output += struct.pack('B', pixel[1])
            output += struct.pack('B', pixel[2])


    blob = open(args.out_file, 'wb')
    blob.write(struct.pack('4s', b'levl'))       # Header
    blob.write(struct.pack('i', img_shape[0]))  # Length of the image
    blob.write(struct.pack('i', img_shape[1]))  # Width of the image
    blob.write(output)                          # Pixel Data
    blob.close()
