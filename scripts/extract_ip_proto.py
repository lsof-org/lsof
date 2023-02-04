import re
import json
import os
protos = dict()
if os.path.exists('ip_proto.json'):
    protos = json.load(open('ip_proto.json', 'r'))

# Extract ip protos from header
path = '/usr/include/netinet/in.h'

# Darwin
if not os.path.exists(path):
    path = '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk' + path
with open(path, 'r') as f:
    for line in f:
        m = re.search('IPPROTO_([A-Z0-9_]+) = ([0-9]+)', line)
        if m:
            name = m[1]
            value = int(m[2])
            protos[name] = value
        else:
            m = re.search('#define\s+IPPROTO_([A-Z0-9_]+)\s+([0-9]+)', line)
            if m:
                name = m[1]
                value = int(m[2])
                protos[name] = value

# BSD Private, FreeBSD has 258 while Darwin has 254
del protos["DIVERT"]
json.dump(protos, open('ip_proto.json', 'w'))
