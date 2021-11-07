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
    pprint(entry_copy1)
    entry_copy2 = deepcopy(entry_copy1)
    entry_copy2['shared'] = False
    entry_copy2['libtype'] = 'dynamic'
    pprint(entry_copy2)
