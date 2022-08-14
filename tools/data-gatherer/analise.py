import json
from math import ceil, sqrt
from statistics import median
import matplotlib
import matplotlib.pyplot as plt

# data = json.loads(open('data.json', 'r').read())
# items = data['monsters']
# m_names = []
# values = []
# i = 0
# dataset = []
# for mname, vs in items.items():
#     if len(vs) == 0:
#         continue
#     value = sum(vs) / len(vs)
#     data['monsters'][mname] = value
#     # value = median(vs)
#     # m_names += [mname]
#     # values += [value]
#     dataset += [[mname, value]]

# open('data-result.json', 'w').write(json.dumps(data, indent=4))

# dataset = sorted(dataset, key=lambda x: x[1])
# for item in dataset:
#     m_names += [item[0]]
#     values += [item[1]]

# fig = plt.figure(figsize = (20, 7))

# # plt.bar(m_names, values, color='green', width=0.4)
# plt.xticks(rotation='vertical')
# plt.bar(m_names, values)
# plt.show()


data = json.loads(open('data.json', 'r').read())
mdata = json.loads(open('monsters.json', 'r').read())
items = data['monsters']
items_res = {}
for mname, pools in items.items():
    if not mname in mdata: continue
    # filter all the item pools that are bigger than average item count
    # av_item_count = mdata[mname] ** 0.4
    actual_pool = []
    for pool in pools:
        l = len(pool)
        # if l <= av_item_count:
        actual_pool += [pool]
    for pool in actual_pool:
        v = mdata[mname] / len(pool)
        for item in pool:
            if not item in items_res:
                items_res[item] = []
            items_res[item] += [v]
res = []
for name, v in items_res.items():
    res += [[name, v]]
res = sorted(res, key=lambda x : sum(x[1]))
names = []
values = []
for pair in res:
    names += [pair[0]]
    values += [sum(pair[1])]

fig = plt.figure(figsize = (20, 7))
font = {'size': 5}

matplotlib.rc('font', **font)

plt.xticks(rotation='vertical')
plt.bar(names, values)
plt.show()

