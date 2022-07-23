from PIL import Image
from os import listdir
from os.path import splitext

SEEK_EXT = '.bmp'
TARGET_EXT = '.bmp'
SCALE = 0.2

# 87 × 121

for file in listdir('.'):
    name, ext = splitext(file)
    if ext != SEEK_EXT:
        continue
    img = Image.open(file)
    width, height = img.size
    # new_size = (width * SCALE, height * SCALE)
    img.save(name + '_large' + ext)
    new_size = (87 , 121)
    img.thumbnail(new_size, Image.ANTIALIAS)
    img.save(file)