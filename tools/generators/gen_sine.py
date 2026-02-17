import math

number_of_items = 256
amplitude = 96

print(','.join("{:3d}".format(int(math.sin((i / number_of_items) * (2 * math.pi)) * (amplitude / 2))) for i in range(number_of_items)))
