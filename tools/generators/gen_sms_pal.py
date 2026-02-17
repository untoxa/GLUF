#!/usr/bin/env python3
import sys
from PIL import Image, ImageDraw

def hexdump(b, w=32):
    print("\n".join(b[i:i + w].hex() for i in range(0, len(b), w)))

def render_to_texels(twidth=16):
    im = Image.new('P', (8 * 8, 8 * 8))
    draw = ImageDraw.Draw(im)

    brightness = [0b000000, 0b000011, 0b001111, 0b111111]
    palette = []
    color = 0
    for y in range(8):
        for x in range(8):        
            red   = ((color & 0b110000) << 2) | brightness[(color & 0b110000) >> 4]
            green = ((color & 0b001100) << 4) | brightness[(color & 0b001100) >> 2]
            blue  = ((color & 0b000011) << 6) | brightness[(color & 0b000011) >> 0]
            palette += [red, green, blue]

            draw.rectangle([x * 8, y * 8, (x + 1) * 8, (y + 1) * 8], fill=color)
            
            color += 1

    im.putpalette(bytes(palette))
    return im


def main(argv=None):
    argv = argv or sys.argv
    outfilename = argv[1] if len(argv) > 1 else None
    twidth = 32
    tiles = render_to_texels(twidth)
    if outfilename:
        tiles.save(outfilename)
    else:
        tiles.show()

if __name__=='__main__':
    if 'idlelib' in sys.modules:
        main(['./romusage.py', '../gb240p.gb'])
    else:
        main()
