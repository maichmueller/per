import os
import json
from copy import deepcopy

d = []
with open(os.path.join(os.getcwd(), '.github', 'workflows', 'build-matrix.json')) as json_data:
    d.extend(json.load(json_data))

new_data = []
for entry in d:
    entry_copy1 = deepcopy(entry)
    # entry_copy1['shared'] = "FALSE"
    # entry_copy1['libtype'] = 'static'
    new_data.append(entry_copy1)
    # entry_copy2 = deepcopy(entry_copy1)
    # entry_copy2['shared'] = "TRUE"
    # entry_copy2['libtype'] = 'dynamic'
    # new_data.append(entry_copy2)

# sort by platform-compiler name
new_data = list(reversed(sorted(new_data, key=lambda x: x["name"])))

with open(os.path.join(os.getcwd(), '.github', 'workflows', 'build-matrix.json'), 'w') as f:
    json.dump(new_data, f)
