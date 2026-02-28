#!/usr/bin/env python3
import sys
from pathlib import Path
from optparse import OptionParser
from PIL import Image
import itertools
from functools import reduce

def extract_tile(pixels, x, y, bits, height, pal = None):
    tile = []
#    pal = [3, 2, 1, 0]
    width = 8
    for dy in range(height):
        a = b = c = d = 0
        w = 0
        for dx in range(8):
            px = pixels[(x * 8) + dx, (y * height) + dy] % (1 << bits)
            if (pal != None):
                f, idx = (True, pal[px]) if px in range(len(pal)) else (False, 0)
            else:
                f, idx = (True, px) if px in range(1 << bits) else (False, 0)            
            a = a << 1 | (idx >> 0) & 0x01
            b = b << 1 | (idx >> 1) & 0x01
            c = c << 1 | (idx >> 2) & 0x01
            d = d << 1 | (idx >> 3) & 0x01
            w += 1 if f else 0
        width = min(width, w)
        tile.append(a); 
        if (bits >= 2): 
            tile.append(b)
        if (bits == 4): 
            tile.append(c)
            tile.append(d)
    return width, tile

def output_array(outf, name, array, width):
    outf.write(bytes("static const uint8_t {0:s}[] = {{\n".format(name),"ascii"))
    idx = 0
    for v in array:
        outf.write(bytes("0x{:02x},".format(v), "ascii"))
        if idx >= (width - 1):
            outf.write(b"\n")
            idx = 0
        else:
            idx += 1
    if (idx != 0): outf.write(b"\n")
    outf.write(b"};\n")


def main():
    parser = OptionParser("Usage: parallax.py [options] INPUT_FILE_NAME.PNG")
    parser.add_option("-s",  '--src',        dest='srcfilename',                                       help='output file name')
    parser.add_option("-o",  '--out',        dest='outfilename',                                       help='output file name')
    parser.add_option("-i",  '--identifier', dest='identifier',                                        help='source identifier')
    parser.add_option("-b",  '--bank',       dest='bank',        default="255",                        help='BANK number (default AUTO=255)')    
    parser.add_option("-p",  '--bpp',        dest='bpp',         default="2",                          help='bits per pixel (default 2)')    
    parser.add_option("-l",  '--list',       dest='tilelist',    default="0",                          help='comma-separated list of metatile numbers')    

    (options, args) = parser.parse_args()

    if (len(args) == 0) and (options.srcfilename == None):
        parser.print_help()
        parser.error("Input file name required\n")
    
    if options.srcfilename == None:
        infilename = Path(args[0])
    else:
        infilename = Path(options.srcfilename)

    if options.outfilename == None:
        outfilename = infilename.with_suffix('.c')
    else:
        outfilename = Path(options.outfilename)
        
    if options.identifier == None:
        identifier = str(Path(infilename.name).with_suffix(''))
    else: 
        identifier = options.identifier

    bits_per_pixel = int(options.bpp)
            
    sample_width = 16
    sample_height = 16
    
    tilelist = [int(x.strip()) for x in options.tilelist.split(',')]    

    series = []

    with Image.open(infilename) as source:
        # get sample from the source image
        for tilenum in tilelist:
            with Image.new('P', (sample_width * 2, sample_height * 2)) as tiled:
                tiled.putpalette(source.getpalette())
        
                # "tilefy" it
                cropped_region = source.crop((tilenum * sample_width, tilenum * sample_height, (tilenum + 1) * sample_width, (tilenum + 1) * sample_height))
                for y in range(2):
                    for x in range(2):
                        tiled.paste(cropped_region, (x * sample_width, y * sample_height))
        
                with Image.new('P', ((sample_width * sample_width), tiled.height)) as result:
                    result.putpalette(source.getpalette())
        
                    # create series of the result image, scrolling each sample by 1
                    for x in range(sample_width):
                        result.paste(tiled.crop((sample_width - x, 0, (sample_width - x) + sample_width, tiled.height)), (x * sample_width, 0))
        
                    total_tiles = sample_width * sample_width
        
                    # create tile data from series
                    idx = 0
                    tiles = list()
                    pixels = result.load()              
                    for x in range(result.width // 8):
                        width, tile = extract_tile(pixels, x, 0, bits_per_pixel, result.height)
                        tiles.append(tile)
                        
            series.append(tiles)

    #export data
    with open(str(outfilename), "wb") as outf:                
        outf.write(bytes("#pragma bank {1:s}\n\n"
                         "#include <stdint.h>\n"
                         "#include <gbdk/platform.h>\n\n"
                         "BANKREF({0:s})\n\n".format(identifier, options.bank), "ascii")
                   )
        for n in range(len(series)):
            outf.write(bytes("const uint8_t {0:s}_{1:d}[] = {{\n"
                             "\t{2:s}\n"
                             "}};\n".format(identifier, n,
                                            ',\n\t'.join(', '.join('0x{:02x}'.format(v) for v in row if v is not None) for row in series[n])
                                            ), "ascii")
                       )
            
    with open(str(outfilename.with_suffix('.h')), "wb") as hdrf:
        hdrf.write(bytes("#ifndef __INCLUDE_{0:s}_H__\n"
                         "#define __INCLUDE_{0:s}_H__\n\n"
                         "#include <stdint.h>\n"
                         "#include <gbdk/platform.h>\n\n"
                         "BANKREF_EXTERN({0:s})\n\n"
                         "{1:s}\n\n"
                         "#endif\n".format(identifier,
                                           '\n'.join('extern const uint8_t {0:s}_{1:d}[];'.format(identifier, n) for n in range(len(series))) 
                                           ), "ascii")
                   )

    return

if __name__=='__main__':
    main()
