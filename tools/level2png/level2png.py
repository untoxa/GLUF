#!/usr/bin/env python3
import sys
from pathlib import Path
from optparse import OptionParser
import itertools
from PIL import Image

def bin2c():
    parser = OptionParser("Usage: drawlevel.py [options] INPUT_FILE_NAME.PNG [TILES_FILE_NAME.PNG]")
    parser.add_option("-o",  '--out',        dest='outfilename',                                       help='output file name')
    parser.add_option("-w",  '--width',      dest='width',       default="12",                         help='array width')

    (options, args) = parser.parse_args()

    if (len(args) == 0):
        parser.print_help()
        parser.error("Input file name required\n")
    
    infilename = Path(args[0])
    if (len(args) > 1):
        intilesname = Path(args[1])
    else:
        intilesname = "tiles.png"
            
    with open(str(infilename), "rb") as inf:
        rows = list(itertools.zip_longest(*[iter(inf.read())] * int(options.width), fillvalue=None))

        tiles = Image.open(intilesname)

        im = Image.new('P', (16 * int(options.width), 16 * len(rows)))
        im.putpalette(tiles.getpalette())
        
        x = y = 0
        for row in rows:
            for v in row:
                cropped_region = tiles.crop((v * 16, 0, (v + 1) * 16, 16))
                im.paste(cropped_region, (x * 16, y * 16, (x + 1) * 16, (y + 1) * 16))
                x += 1
            y += 1
            x = 0
            
        if (options.outfilename):
            im.save(options.outfilename)
        else:
            im.show()

    return

if __name__=='__main__':
    bin2c()
