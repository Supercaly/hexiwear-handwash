# This python3 script reads .bmp images and generate a .c file with
# their data as c-style arrays.
#
# Note: This script works only with .bmp images that uses the following format:
#   DIB header:      BITMAPINFOHEADER
#   size:            96x96
#   bit per pixel:   24
#   compression:     BI_RGB

import sys
import os
from pathlib import Path
import argparse

# converts list of bytes to int
def bytes_to_int(bs):
    return int.from_bytes([int.from_bytes(a, 'little') for a in bs], 'little')
    
# flip vertically a bytes list
def flip_vertical(bytes):
    res = []
    r = 95
    c = 0
    while r >= 0:
        while c < 192:
            b1 = bytes[r*192+c]
            res.append(b1)
            c+=1
        r-=1
        c = 0
    return res

# process a bmp image with given raw bytes and returns his bytes-array
def raw_bytes_from_bmp(bs):
    # remove unused BMP header
    bs = bs[14:]

    # read DIB header name
    dib_header = bytes_to_int(bs[:4])
    print(f"DIB header:  {dib_header}")

    # read full DIB header
    full_dib_header = bs[:dib_header]

    # read image of version BITMAPINFOHEADER 
    if dib_header == 40:
        print("  DIB header type: BITMAPINFOHEADER")

        w = bytes_to_int(full_dib_header[4:8])
        h = bytes_to_int(full_dib_header[8:12])
        bit_count = bytes_to_int(full_dib_header[14:16])
        compression = bytes_to_int(full_dib_header[16:20])
        size = bytes_to_int(full_dib_header[20:24])
        hres = bytes_to_int(full_dib_header[24:28])
        vres = bytes_to_int(full_dib_header[28:32])
        colors = bytes_to_int(full_dib_header[32:36])

        print(f"  size:            {w}x{h}")
        print(f"  bit per pixel:   {bit_count}")
        print(f"  compression:     {compression}")
        print(f"  image size:      {size}")
        print(f"  pixel per meter: {hres}x{vres}")
        print(f"  colors:          {colors}")

        raw = bs[40:]
        bs = bs[size:]
        
        print(f"  data bytes: {len(raw)}")
        print(f"  Remaining bytes: {len(bs)}")

        if bit_count != 24:
            print(f"ERROR: bmp image has {bit_count}bpp, but must have 24bpp to be processed!")
            sys.exit(1)

        # map 24-bit R8G8B8 to 16-bit R5G6B5
        result = []
        row = 0
        col = 0
        while row < 96:
            while col < 288:
                r = int.from_bytes(raw[row*288+col+2], 'little')
                g = int.from_bytes(raw[row*288+col+1], 'little')
                b = int.from_bytes(raw[row*288+col+0], 'little')
                rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | ((b & 0b11111000) >> 3)
                result.append(((rgb >> 8) & 0xff).to_bytes(1,'little'))
                result.append((rgb & 0xff).to_bytes(1,'little'))
                col+=3
            col = 0
            row+=1
        return flip_vertical(result)
    else:
        print(f"ERROR: unknown dib header {dib_header}")
        sys.exit(1)

# format raw data as a string with a c array
def raw_data_to_c_array(var_name, bytes):
    result = "const uint8_t "+var_name+"[IMAGE_SIZE] = {"
    for i,b in enumerate(bytes):
        if i % 16 == 0:
            result+="\n\t"
        result+="0x"+b.hex()+","
    result+="\n};\n"
    return result

# format a string with c array declaration
def c_array_decl(var_name):
    return "extern const uint8_t "+var_name+"[IMAGE_SIZE];\n"

# reads raw bytes from a bmp file with given name
def read_bmp_file(file_name):
    with open(file_name, 'rb') as bmp_file:
        bytes = []
        byte = bmp_file.read(1)
        while byte:
            bytes.append(byte)
            byte = bmp_file.read(1)
    return bytes

def var_name_from_file(file_name):
    return f"{Path(file_name).stem.lower()}_bmp"

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
    all_data = [(var_name_from_file(file),raw_bytes_from_bmp(read_bmp_file(file))) for file in bmp_files]

    Path(args.output).mkdir(parents=True,exist_ok=True)

    # generate output .h file
    h_file = open(os.path.join(args.output,args.name+".h"), "w+")
    h_file.write("#ifndef OUTPUT_H_\n")
    h_file.write("#define OUTPUT_H_\n")
    h_file.write("\n")
    h_file.write("#include <stdint.h>\n")
    h_file.write("\n")
    h_file.write("#define IMAGE_SIZE (96*96*2)\n")
    h_file.write("\n")
    for var in all_data:
        h_file.write(c_array_decl(var[0]))
        h_file.write("\n")
    h_file.write("#endif // OUTPUT_H_")
    h_file.close()
    
    # generate output .c file
    c_file = open(os.path.join(args.output,args.name+".c"), 'w+')
    c_file.write(f"#include \"{args.name}.h\"\n")
    c_file.write("\n")
    for var in all_data:
        c_file.write(raw_data_to_c_array(var[0],var[1]))
        c_file.write("\n")
    c_file.close()
