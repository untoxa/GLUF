#!/usr/bin/env python3
import sys
from pathlib import Path
from optparse import OptionParser
import itertools

def bin2c():
    parser = OptionParser("Usage: png2metatiles.py [options] INPUT_FILE_NAME.PNG")
    parser.add_option("-o",  '--out',        dest='outfilename',                                       help='output file name')
    parser.add_option("-i",  '--identifier', dest='identifier',                                        help='source identifier')
    parser.add_option("-w",  '--width',      dest='width',       default="16",                         help='array width')
    parser.add_option("-b",  '--bank',       dest='bank',        default="255",                        help='BANK number (default AUTO=255)')    

    (options, args) = parser.parse_args()

    if (len(args) == 0):
        parser.print_help()
        parser.error("Input file name required\n")
    
    infilename = Path(args[0])
    
    if options.outfilename == None:
        outfilename = infilename.with_suffix('.c')
    else:
        outfilename = Path(options.outfilename)
        
    if options.identifier == None:
        identifier = str(Path(infilename.name).with_suffix(''))
    else: 
        identifier = options.identifier

    with open(str(infilename), "rb") as inf:
        rows = list(itertools.zip_longest(*[iter(inf.read())] * int(options.width)))
        with open(str(outfilename), "wb") as outf:                
            outf.write(bytes("#pragma bank {1:s}\n\n"
                             "#include <stdint.h>\n"
                             "#include <gbdk/platform.h>\n\n"
                             "BANKREF({0:s})\n\n"
                             "const uint8_t {0:s}[] = {{\n"
                             "\t{2:s}\n"
                             "}};\n".format(identifier, options.bank,
                                            ',\n\t'.join(', '.join('0x{:02x}'.format(v) for v in row if v is not None) for row in rows)
                                            ), "ascii")
                       )
        with open(str(outfilename.with_suffix('.h')), "wb") as hdrf:
            hdrf.write(bytes("#ifndef __INCLUDE_{0:s}_H__\n"
                             "#define __INCLUDE_{0:s}_H__\n\n"
                             "#include <stdint.h>\n"
                             "#include <gbdk/platform.h>\n\n"
                             "BANKREF_EXTERN({0:s})\n"
                             "extern const uint8_t {0:s}[];\n\n"
                             "#endif\n".format(identifier), "ascii")
                       )
    return

if __name__=='__main__':
    bin2c()
