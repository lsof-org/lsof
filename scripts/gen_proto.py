# Generate lsof_protocol enum from IP and Ethernet protocols

import json
names = []
with open('lsof_proto.h', 'w') as f:
    print('enum lsof_protocol {', file=f)
    ip_protos = json.load(open('ip_proto.json', 'r'))
    protos = ip_protos.items()
    print('    /* Follow ip protocol numbers order */', file=f)
    for proto in sorted(protos, key=lambda p: p[1]):
        print(
            f'    LSOF_PROTOCOL_{proto[0]},    /**< {proto[0]}({proto[1]}) */', file=f)
        names.append(proto[0])

    print('    /* Follow ethernet type order */', file=f)
    eth_protos = json.load(open('eth_proto.json', 'r'))
    for proto in eth_protos:
        if not proto[0] in names:
            print(
                f'    LSOF_PROTOCOL_{proto[0]},    /**< {proto[0]}(0x{proto[1]:04X}) */', file=f)
            names.append(proto[0])
    print('};', file=f)

rename = {
    "FRAGMENT": "FRAGMNT",
    "LOCALTALK": "LCLTALK",
    "ETHERNET": "ETHER",
    "TR_802_2": "802.2",
    "IEEE802154": "802154",
    "802_3_MIN": "802.3",
    "IEEEPUPAT": "I3EPUPAT",
    "PPP_DISC": "PPPDISC",
    "LINK_CTL": "LINKCTL",
    "PROFINET": "PROFINT",
    "ETHERCAT": "ETHCAT",
    "LOOPBACK": "LOOPBAK",
    "DSA_8021Q": "DSAD1Q",
    "DSA_A5PSW": "DSA5PSW",
    "SATEXPAK": "STEXPAK",
    "KRYPTOLAN": "KRYPTLN",
    "BRSATMON": "BRSTMON",
    "RESERVED_253": "RSVD253",
    "RESERVED_254": "RSVD254",
    "OLD_DIVERT": "ODIVERT",
}

with open('lsof_proto.c', 'w') as f:
    print('void print_iproto() {', file=f)
    print('    switch (proto) {', file=f)
    for name in names:
        visible = name
        if len(name) > 7:
            if name in rename:
                visible = rename[name]
            else:
                print(f'{name} too long!')
        print(f'    case LSOF_PROTOCOL_{name}:', file=f)
        print(f'        (void)snpf(buf, buf_len, "{visible}");', file=f)
        print('        break;', file=f)
    print('    }', file=f)
    print('}', file=f)
