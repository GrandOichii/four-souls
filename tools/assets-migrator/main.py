import json
from os.path import join, exists
from os import listdir, makedirs
from shutil import copy2

GAME_PATH = '../../game'
RESULT_DIR = 'assets'
RESULT_FILE = {
    'font': 'font.ttf',
    'cards': {}
}

def filterName(name):
    result = name
    result = result.replace(' ', '_')
    result = result.replace('’', '_')
    result = result.replace('\'', '_')
    result = result.replace('.', '_')
    result = result.replace('!', '_')
    return result.lower()

makedirs(RESULT_DIR, exist_ok=True)
IMAGES_DIR = 'images'
makedirs(join(RESULT_DIR, IMAGES_DIR), exist_ok=True)

# read loot
cards_info = json.loads(open(join(GAME_PATH, 'loot.json'), 'r').read())
back_p = cards_info['back']
cards_p = list(cards_info['cards']['basic'].keys()) + list(cards_info['cards']['trinkets'].keys())
for i in range(len(cards_p)): cards_p[i] = join(GAME_PATH, cards_p[i])

RESULT_FILE['loot_back'] = cards_info['back']
copy2(join(GAME_PATH, back_p), join(RESULT_DIR, IMAGES_DIR))
for card_p in cards_p:
    info = json.loads(open(join(card_p, 'card.json'), 'r').read())
    name = info['name']
    filteredName = filterName(name)
    im = join(card_p, 'image.bmp')
    res = {}
    if exists(im):
        imageP = filteredName + '.bmp'
        res['small'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    im = join(card_p, 'image_large.bmp')
    if exists(im):
        imageP = filteredName + '_large.bmp'
        res['large'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))

    RESULT_FILE['cards'][name] = res

    print(info['name'])

# read monsters
cards_info = json.loads(open(join(GAME_PATH, 'monsters.json'), 'r').read())
back_p = cards_info['back']
cards_p = list(cards_info['cards']['monsters'])
for i in range(len(cards_p)): cards_p[i] = join(GAME_PATH, cards_p[i])

RESULT_FILE['monster_back'] = cards_info['back']
copy2(join(GAME_PATH, back_p), join(RESULT_DIR, IMAGES_DIR))
for card_p in cards_p:
    info = json.loads(open(join(card_p, 'card.json'), 'r').read())
    name = info['name']
    filteredName = filterName(name)
    im = join(card_p, 'image.bmp')
    res = {}
    if exists(im):
        imageP = filteredName + '.bmp'
        res['small'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    im = join(card_p, 'image_large.bmp')
    if exists(im):
        imageP = filteredName + '_large.bmp'
        res['large'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))

    RESULT_FILE['cards'][name] = res

    print(info['name'])


# read treasure
cards_info = json.loads(open(join(GAME_PATH, 'treasures.json'), 'r').read())
back_p = cards_info['back']
cards_p = list(cards_info['cards'])
for i in range(len(cards_p)): cards_p[i] = join(GAME_PATH, cards_p[i])

RESULT_FILE['treasure_back'] = cards_info['back']
copy2(join(GAME_PATH, back_p), join(RESULT_DIR, IMAGES_DIR))
for card_p in cards_p:
    info = json.loads(open(join(card_p, 'card.json'), 'r').read())
    name = info['name']
    filteredName = filterName(name)
    im = join(card_p, 'image.bmp')
    res = {}
    if exists(im):
        imageP = filteredName + '.bmp'
        res['small'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    im = join(card_p, 'image_large.bmp')
    if exists(im):
        imageP = filteredName + '_large.bmp'
        res['large'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))

    RESULT_FILE['cards'][name] = res

    print(info['name'])

# read characters
cards_info = json.loads(open(join(GAME_PATH, 'characters.json'), 'r').read())
cards_p = list(cards_info)
for i in range(len(cards_p)): cards_p[i] = join(GAME_PATH, cards_p[i])

copy2(join(GAME_PATH, back_p), join(RESULT_DIR, IMAGES_DIR))
for card_p in cards_p:
    info = json.loads(open(join(card_p, 'card.json'), 'r').read())
    name = info['name']
    filteredName = filterName(name)
    im = join(card_p, 'image.bmp')
    res = {}
    if exists(im):
        imageP = filteredName + '.bmp'
        res['small'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    im = join(card_p, 'image_large.bmp')
    if exists(im):
        imageP = filteredName + '_large.bmp'
        res['large'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    card_p = join(card_p, info['item'])
    info = json.loads(open(join((card_p), 'card.json'), 'r').read())
    name = info['name']
    filteredName = filterName(name)
    im = join(card_p, 'image.bmp')
    res = {}
    if exists(im):
        imageP = filteredName + '.bmp'
        res['small'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))
    im = join(card_p, 'image_large.bmp')
    if exists(im):
        imageP = filteredName + '_large.bmp'
        res['large'] = join(IMAGES_DIR, imageP)
        if not exists(join(RESULT_DIR, IMAGES_DIR, imageP)):
            copy2(im, join(RESULT_DIR, IMAGES_DIR, imageP))


    RESULT_FILE['cards'][name] = res

    print(info['name'])

open(join(RESULT_DIR, 'config.json'), 'w').write(json.dumps(RESULT_FILE, indent=4))

