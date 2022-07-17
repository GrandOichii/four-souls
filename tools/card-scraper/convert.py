import json
from PIL import Image

from os import makedirs, listdir
from os.path import join

ITEMS = json.loads(open('result-monsters1.json', 'r').read())['monster']
IMAGES_PATH = '../../images'
OUT_PATH = 'monsters'
IMAGE_PATHS = listdir(IMAGES_PATH)
MONSTERS_FILE_CONTENTS = []
MISSING_IMAGES = []

def filterName(name: str):
    result = name
    result = result.replace(' ', '_')
    result = result.replace('’', '_')
    result = result.replace('.', '_')
    result = result.replace('!', '_')
    return result.lower()

def saveMonster(monster: dict):
    filteredName = filterName(monster['name'])
    dirName = str(filteredName)
    filteredName = filteredName.replace('_', '')
    pf = '__missing____missing____missing____missing____missing__'
    for p in IMAGE_PATHS:
        if filteredName in p.lower() and len(p) < len(pf):
            pf = p
    if pf == '__missing____missing____missing____missing____missing__':
        global MISSING_IMAGES
        MISSING_IMAGES += [monster['name']]
        return '', False
    resultName = join(OUT_PATH, dirName)
    dirName = join('monsters', resultName)
    print(dirName)
    pf = join(IMAGES_PATH, pf)
    print(pf)
    makedirs(dirName, exist_ok=True)
    # write card.json
    card = {}
    card['name'] = monster['name'][0]
    for i in range(1, len(monster['name'])):
        if monster['name'][i-1] == ' ':
            card['name'] += monster['name'][i]
        else:
            card['name'] += monster['name'][i].lower()
    card['text'] = ''
    card['image'] = 'image.bmp'
    card['script'] = 'script.lua'
    card['health'] = monster['health']
    card['roll'] = monster['roll']
    card['power'] = monster['power']
    card['rewards'] = card['name'].replace(' ', '').replace('’', '').replace('.', '').replace('!', '') + '_rewards'
    open(join(dirName, 'card.json'), 'w').write(json.dumps(card, indent=4))
    # write lua script
    open(join(dirName, 'script.lua'), 'w').write('function ' + card['rewards'] + '(host, me, killer)\n\t--  TODO\nend')
    # write image
    img = Image.open(pf)
    img.save(join(dirName, 'image_large.bmp'))
    # new_size = (width * SCALE, height * SCALE)
    new_size = (87 , 121)
    img.thumbnail(new_size, Image.ANTIALIAS)
    img.save(join(dirName, 'image.bmp'))
    return resultName, True

makedirs(OUT_PATH, exist_ok=True)
makedirs(join(OUT_PATH, 'monsters'), exist_ok=True)
for monster in ITEMS:
    name, saved = saveMonster(monster)
    if not saved: continue
    MONSTERS_FILE_CONTENTS += [name]
# print(len(ITEMS))

open(join(OUT_PATH, 'monsters.json'), 'w').write(json.dumps(MONSTERS_FILE_CONTENTS, sort_keys=True, indent=4))
open('missing.json', 'w').write(json.dumps(MISSING_IMAGES, sort_keys=True, indent=4))