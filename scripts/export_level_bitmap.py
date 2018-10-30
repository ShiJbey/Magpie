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

MESH_ID = {
    0: 'empty',
    1: 'magpie',
    2: 'guard',
    3: 'floor',
    4: 'wall'
}

COLLIDABLE_MESH_MASK = int('11110000', 2)
COLLIDABLE_MESH_OFFSET = 4

NONCOLLIDABLE_MESH_MASK = int('00001111', 2)
NONCOLLIDABLE_MESH_OFFSET = 0

ROOM_NUMBER_MASK = int('11110000', 2)
ROOM_NUMBER_OFFSET = 4

GUARD_PATH_MASK = int('00001000', 2)
GUARD_PATH_OFFSET = 3

OBJECT_ID_MASK = int('11110000', 2)
OBJECT_ID_OFFSET = 4

INTERACTION_FUNC_ID_MASK = int('00001100', 2)
INTERACTION_FUNC_ID_OFFSET = 2

INTERACTION_FLAG_MASK = int('00000011', 2)
INTERACTION_FLAG_OFFSET = 0

def get_mesh_id(red_channel_data):
    collidable_mesh_id = (red_channel_data & COLLIDABLE_MESH_MASK) >> COLLIDABLE_MESH_OFFSET
    noncollidable_mesh_id = (red_channel_data & NONCOLLIDABLE_MESH_MASK) >> NONCOLLIDABLE_MESH_OFFSET
    return collidable_mesh_id, noncollidable_mesh_id

def is_guard_path(green_channel_data):
    return (green_channel_data & GUARD_PATH_MASK) >> GUARD_PATH_OFFSET

def get_room_number(green_channel_data):
    return (green_channel_data & ROOM_NUMBER_MASK) >> ROOM_NUMBER_OFFSET

def get_object_id(blue_channel_data):
    return (blue_channel_data & OBJECT_ID_MASK) >> OBJECT_ID_OFFSET

def get_interaction_func_id(blue_channel_data):
    return (blue_channel_data & INTERACTION_FUNC_ID_MASK) >> INTERACTION_FUNC_ID_OFFSET

def get_interaction_flag(blue_channel_data):
    return (blue_channel_data & INTERACTION_FLAG_MASK) >> INTERACTION_FLAG_OFFSET


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

class SingleMeshValidator(AbstractValidator):

    def check(self, pixel_data):
        super().check(pixel_data)
        collidable_mesh_id, noncollidable_mesh_id = get_mesh_id(pixel_data[COLOR_CHANNELS['RED']])
        # Throws an error if we have both a collidable and a non collidable mesh defined
        assert(collidable_mesh_id != 0 and noncollidable_mesh_id != 0)


class GuardPathValidator(AbstractValidator):

    def check(self, pixel_data):
        super().check(pixel_data)
        # Check if this tile is part of a guard path
        # if it is, then make sure that the mesh is a floor tile
        if (is_guard_path(pixel_data[COLOR_CHANNELS['GREEN']])):
            _, noncollidable_mesh_id = get_mesh_id(pixel_data[COLOR_CHANNELS['RED']])
            assert(noncollidable_mesh_id == MESH_ID['floor'])

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
    validators = [ SingleMeshValidator(), GuardPathValidator() ]

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
