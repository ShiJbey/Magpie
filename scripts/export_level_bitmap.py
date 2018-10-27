# Exports a bitmap file containing the level data
# to a bindary format to be imported by c++

import argparse
import cv2
import numpy as np
import struct
import sys


# Abstract Validator class to be extended by all level validators
class AbstractValidator():

    def __init__(self):
        pass

    def check(self, pixel_data):
        """
        This function is supposed to raise an exception when this pixel
        has invalid values
        """
        pass

#############################################################
#                                                           #
#              DEFINE CUSTOM PIXEL VALIDATORS               #
#                                                           #
#############################################################

# For each pixel attribute we want to check, we need to extend
# the AbstractValidator class and implement it's check() method
# be sure to add the validator to the validator list created
# before looping throught the pixels




        
#############################################################
#                                                           #
#              RUN VALIDATION AND EXPORT PIXELS             #
#                                                           #
#############################################################

if (__name__ == "__main__"):
    parser = argparse.ArgumentParser()
    parser.add_argument("img_file", help="path to the bitmap being exported",
    type=str)
    parser.add_argument("out_file", help="path to the directory where the level file will be exported",
    type=str)
    parser.add_argument("--stride", help="how many cells to skip", type=int, default=1)
    args = parser.parse_args()

    img_file = args.img_file
    img = cv2.imread(img_file, cv2.IMREAD_COLOR)
    img_shape = img.shape

    # Validators are classes that ensure that the level
    # encoding doesn't have errors
    validators = []

    # Byte stream to hold output
    output = b''

    # Iterate over all pixels
    for row in range(0, img_shape[0], args.stride):
        for col in range(0, img_shape[1], args.stride):
            
            # By default CV2 is BGR instead of RGB
            pixel = np.flip(img[row, col], axis=0)

            # Run validators on the level encoding
            for validator in validators:
                validator.check(pixel)

            # Write this pixel to a binary file
            output += struct.pack('B', pixel[0])
            output += struct.pack('B', pixel[1])
            output += struct.pack('B', pixel[2])


    blob = open(args.out_file, 'wb')
    blob.write(output)
    blob.close()

    print("Done")
