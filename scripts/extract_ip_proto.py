import re
import json
import os
protos = dict()
if os.path.exists('ip_proto.json'):
    protos = json.load(open('ip_proto.json', 'r'))

# Extract ip protos from header
with open('/usr/include/netinet/in.h', 'r') as f:
    for line in f:
        m = re.search('\s+IPPROTO_([A-Z0-9_]+) = ([0-9]+)', line)
        if m:
            name = m[1]
            value = int(m[2])
            protos[name] = value

json.dump(protos, open('ip_proto.json', 'w'))
