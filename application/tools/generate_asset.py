# This python3 script reads .bmp images and generate a .c file with
# their data as c-style arrays.
#
# Note: This script works only with .bmp images that uses the following format:
#   DIB header:      BITMAPINFOHEADER
#   size:            any
#   bit per pixel:   24
#   compression:     BI_RGB
# 
# The output produced by this script is in the form R5B6G5 and the pixels starts
# from top-left
#
# This script was created following:
# - https://en.wikipedia.org/wiki/BMP_file_format
# - https://github.com/javl/image2cpp

from math import ceil
import sys
import os
from pathlib import Path
import argparse

print_messages = False

# Print a message only if print_messages is True
def message(msg):
    if print_messages:
        print(msg)

# converts list of bytes to int
def bytes_to_int(bs):
    return int.from_bytes([int.from_bytes(a, 'little') for a in bs], 'little')

class Image:
    def __init__(self, path):
        self.path = path
        self.name = Path(path).stem.lower()
        self.load_bmp_file()

    # reads the bmp file for this image and loads it'values in memory
    def load_bmp_file(self):
        bs = []
        # open bmp file
        with open(self.path, 'rb') as bmp_file:
            byte = bmp_file.read(1)
            while byte:
                bs.append(byte)
                byte = bmp_file.read(1)

        # remove unused BMP header
        bs = bs[14:]

        # read DIB header name
        dib_header = bytes_to_int(bs[:4])
        message(f"DIB header:  {dib_header}")

        # read full DIB header
        full_dib_header = bs[:dib_header]

        # read image of version BITMAPINFOHEADER 
        if dib_header == 40:
            message("  DIB header type: BITMAPINFOHEADER")

            self.img_width = bytes_to_int(full_dib_header[4:8])
            self.img_height = bytes_to_int(full_dib_header[8:12])
            self.bit_count = bytes_to_int(full_dib_header[14:16])
            self.compression = bytes_to_int(full_dib_header[16:20])
            self.size = bytes_to_int(full_dib_header[20:24])
            hres = bytes_to_int(full_dib_header[24:28])
            vres = bytes_to_int(full_dib_header[28:32])
            colors = bytes_to_int(full_dib_header[32:36])

            message(f"  size:            {self.img_width}x{self.img_height}")
            message(f"  bit per pixel:   {self.bit_count}")
            message(f"  compression:     {self.compression}")
            message(f"  image size:      {self.size}")
            message(f"  pixel per meter: {hres}x{vres}")
            message(f"  colors:          {colors}")

            raw = bs[40:]
            bs = bs[self.size:]
            
            message(f"  data bytes: {len(raw)}")
            message(f"  Remaining bytes: {len(bs)}")

            if self.bit_count != 24:
                print(f"ERROR: bmp image has {self.bit_count}bpp, but must have 24bpp to be processed!")
                sys.exit(1)

            # map 24-bit R8G8B8 to 16-bit R5G6B5
            result = []
            row = 0
            col = 0
            row_size = ceil(self.bit_count*self.img_width/32)*4
            while row < self.img_height:
                while col < row_size:
                    r = int.from_bytes(raw[row*row_size+col+2], 'little')
                    g = int.from_bytes(raw[row*row_size+col+1], 'little')
                    b = int.from_bytes(raw[row*row_size+col+0], 'little')
                    rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | ((b & 0b11111000) >> 3)
                    result.append(((rgb >> 8) & 0xff).to_bytes(1,'little'))
                    result.append((rgb & 0xff).to_bytes(1,'little'))
                    col+=3
                col = 0
                row+=1
            self.bit_count = 16
            self.size = self.img_width * self.img_height * 2
            self.data = result
            self.flip_vertical()
        else:
            print(f"ERROR: unknown dib header {dib_header}")
            sys.exit(1)

    # flip the image vertically
    def flip_vertical(self):
        row_size = ceil(self.bit_count*self.img_width/32)*4
        res = []
        r = self.img_height-1
        c = 0
        while r >= 0:
            while c < row_size:
                b1 = self.data[r*row_size+c]
                res.append(b1)
                c+=1
            r-=1
            c = 0
        self.data = res

    # returns the var name for this image 
    def var_name(self):
        return self.name+"_bmp"

    def byte_count(self):
        return int(self.bit_count/8)

   
# format raw data as a string with a c array
def c_array_def(image: Image):
    result = f"const uint8_t {image.var_name()}[IMAGE_SIZE_{image.img_width}_{image.img_height}] = {{"
    for i,b in enumerate(image.data):
        if i % 16 == 0:
            result+="\n\t"
        result+="0x"+b.hex()+","
    result+="\n};\n"
    return result

# format a string with c array declaration
def c_array_decl(image: Image):
    return f"extern const uint8_t {image.var_name()}[IMAGE_SIZE_{image.img_width}_{image.img_height}];\n"

if __name__ == "__main__":
    # read command line args
    parser = argparse.ArgumentParser(description="Read .bmp images in a folder and generate a .c file with their data as c-style arrays.")
    parser.add_argument('--input', default="./", help="folder where the bmp assets are stored")
    parser.add_argument('--output', default="./generated/", help="folder where the generated files are saved")
    parser.add_argument('--name', default="output", help="name of the generated .c file")
    args = parser.parse_args()
    
    #list all bmp files inside input folder
    if not os.path.isdir(args.input):
        print(f"ERROR: input directory '{args.input}' doesn't exist")
        sys.exit(1)

    bmp_files = [f for f in os.listdir(args.input) if os.path.isfile(os.path.join(args.input,f)) and f.endswith(".bmp")]
    images = [Image(os.path.join(args.input, name)) for name in bmp_files]
    sizes = list(set([(img.img_width, img.img_height, img.byte_count()) for img in images]))

    Path(args.output).mkdir(parents=True,exist_ok=True)

    # generate output .h file
    guard_name = args.name.upper()+"_H_"
    with open(os.path.join(args.output,args.name+".h"), "w+") as h_file:
        h_file.write(f"#ifndef {guard_name}\n")
        h_file.write(f"#define {guard_name}\n")
        h_file.write("\n")
        h_file.write("#include <stdint.h>\n")
        h_file.write("\n")
        for sz in sizes:
            h_file.write(f"#define IMAGE_SIZE_{sz[0]}_{sz[1]} ({sz[0]} * {sz[1]} * {sz[2]})\n")
        h_file.write("\n")
        for img in images:
            h_file.write(c_array_decl(img))
            h_file.write("\n")
        h_file.write(f"#endif // {guard_name}")
    
    # generate output .c file
    with open(os.path.join(args.output,args.name+".c"), 'w+') as c_file:
        c_file.write(f"#include \"{args.name}.h\"\n")
        c_file.write("\n")
        for img in images:
            c_file.write(c_array_def(img))
            c_file.write("\n")

    print(f"Asset generation completed!")