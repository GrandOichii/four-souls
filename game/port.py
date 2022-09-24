from os import listdir
from os.path import join
import json

CONFIG_PATH = '../assets/config.json'
config = json.loads(open(CONFIG_PATH, 'r').read())
def set_config(name, key):
    global config
    for n in config['cards']:
        if n != name: continue
        v = config['cards'][n]
        config.pop(n, None)
        config['cards'][key] = v
        return

dirs = ['bonus_souls', 'characters', 'loot', 'monsters', 'monsters_bonus', 'monsters_curses', 'treasure']
for dir in dirs:
    cards = listdir(dir)
    for card_key in cards:
        if card_key == '.DS_Store': continue
        p = join(dir, card_key, 'card.json')
        data = json.loads(open(p, 'r').read())
        data['key'] = card_key
        open(p, 'w').write(json.dumps(data, indent=4))
        set_config(data['name'], data['key'])

open(CONFIG_PATH, 'w').write(json.dumps(config, indent=4, sort_keys=True))