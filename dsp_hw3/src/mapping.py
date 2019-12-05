import sys
from collections import defaultdict

mapping = defaultdict(list)

with open(sys.argv[1], 'r', encoding="big5-hkscs") as fin:
    for line in fin:
        char = line[:1]
        keys = set(zhuin[0] for zhuin in line[2:].split('/'))
        mapping[char].append(char)
        for key in keys:
            mapping[key].append(char)

with open(sys.argv[2], 'w', encoding="big5-hkscs") as fout:
    for key, chars in mapping.items():
        fout.write(key)
        fout.write(' ')
        fout.write(' '.join(chars))
        fout.write('\n')
