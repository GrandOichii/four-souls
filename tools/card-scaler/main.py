from PIL import Image
from os import listdir
from os.path import splitext, join

SEEK_EXT = '.bmp'
TARGET_EXT = '.bmp'
SCALE = 0.2

for file in listdir('.'):
    name, ext = splitext(file)
    if ext != SEEK_EXT:
        continue
    img = Image.open(file)
    width, height = img.size
    new_size = (width * SCALE, height * SCALE)
    img.thumbnail(new_size, Image.ANTIALIAS)
    img.save('scaled_' + file)