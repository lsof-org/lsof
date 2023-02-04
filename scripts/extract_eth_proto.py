import re
import json
protos = []
# Extract ethernet protos from linux header
with open('/usr/include/linux/if_ether.h', 'r') as f:
    for line in f:
        if re.match('#define\sETH_P_', line):
            parts = re.split('\s', line)
            parts = list(filter(lambda v: len(v) > 0, parts))
            name = parts[1][6:]
            value = int(parts[2], 16)
            protos.append((name, value))

protos = sorted(protos, key=lambda p: p[1])
print(protos)
json.dump(protos, open('eth_proto.json', 'w'))
