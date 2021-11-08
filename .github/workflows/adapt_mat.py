import os
import json
from copy import deepcopy
from pprint import pprint

d = []
with open(os.path.join(os.getcwd(), '.github', 'workflows', 'build-matrix.json')) as json_data:
    d.extend(json.load(json_data))

new_data = []
for entry in d:
    entry_copy1 = deepcopy(entry)
    entry_copy1['shared'] = False
    entry_copy1['libtype'] = 'static'
    new_data.append(entry_copy1)
    entry_copy2 = deepcopy(entry_copy1)
    entry_copy2['shared'] = False
    entry_copy2['libtype'] = 'dynamic'
    new_data.append(entry_copy2)
print("[")
for i, entry in enumerate(new_data):
    print("{")
    for k, v in entry.items():
        print(f'"{k}":"{v}"')
    if i == (len(new_data) - 1):
        print("}")
    else:
        print("},")
print("]")
