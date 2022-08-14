import json
import os

DATA_PATH = 'data.json'
NEW_DATA_PATH = 'new-data.json'

if not os.path.exists(DATA_PATH):
    data = {
        "monsters": {}
    }
    open(DATA_PATH, 'w').write(json.dumps(data))

data = json.loads(open(DATA_PATH, 'r').read())
new_data = json.loads(open(NEW_DATA_PATH, 'r').read())
for mname, value in new_data['monsters'].items():
    if not mname in data['monsters']:
        data['monsters'][mname] = []
    data['monsters'][mname] += [value]
open(DATA_PATH, 'w').write(json.dumps(data, indent=4))
os.remove(NEW_DATA_PATH)