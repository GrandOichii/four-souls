from PIL import Image
from os import listdir
from os.path import splitext, join

IN_DIR = '.'
OUT_DIR = 'result'
EXT = '.bmp'

for file in listdir(IN_DIR):
    name, ext = splitext(file)
    if ext != EXT:
        continue
    img = Image.open(join(IN_DIR, file))
    pixdata = img.load()
    y = int(img.size[1] / 2)
    xc = 0
    while True:
        if pixdata[xc, y] != (255, 255, 255):
            break
        xc+=1

    x = int(img.size[0] / 2)
    yc = 0
    while True:
        if pixdata[x, yc] != (255, 255, 255):
            break
        yc+=1
    img.crop((xc, yc, img.size[0]-xc, img.size[1]-yc)).save(join(OUT_DIR, file))
    print(name + ' cropped')

