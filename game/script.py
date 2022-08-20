import json
from os import listdir
from os.path import join, isfile

# DIRS = ['loot', 'treasure']
# for dir in DIRS:
#     cards = listdir(dir)
#     for card in cards:
#         fpath = join(dir, card, 'script.lua')
#         lines = open(fpath, 'r').read().split('\n')
#         for i in range(len(lines)):
#             line = lines[i]
#             if '_enter(host, me, owner)' in line:
#                 lines[i] = line.replace('_enter(host, me, owner)', '_enter(host)')
#                 lines.insert(i+1, '\tlocal me = this(host)')
#                 lines.insert(i+1, '\tlocal owner = getTopOwner(host)')
#                 print(line.replace('_enter(host, me, owner)', '_enter(host)'))
#         open(fpath, 'w').write('\n'.join(lines))

cards = listdir('characters')
for card in cards:
    fpath = join('characters', card, 'card.json')
    # TODO
    data = json.loads(open(fpath, 'r').read())
    if not 'use' in data: continue
    if str(type(data['use'])) == '<class \'str\'>':
        data['use'] = {
            'effect': data['use']
        }
        if 'cost' in data:
            data['use']['cost'] = data['cost']
            data.pop('cost', None)
        open(fpath, 'w').write(json.dumps(data, indent=4))
    # lines = open(fpath, 'r').read().split('\n')
    # for i in range(len(lines)):
    #     line = lines[i]
    #     if '_rewards(host, me, killer)' in line:
    #         lines[i] = line.replace('_rewards(host, me, killer)', '_rewards(host)')
    #         lines.insert(i+1, '\tlocal killer = Common_PlayerWithID(host, killerID)')
    #         lines.insert(i+1, '\tlocal killerID = getLastKillerID(host)')
    #         # print(line.replace('_enter(host, me, owner)', '_enter(host)'))
    # open(fpath, 'w').write('\n'.join(lines))