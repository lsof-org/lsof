void print_iproto() {
    switch (proto) {
    case LSOF_PROTOCOL_IP:
        (void)snpf(buf, buf_len, "IP");
        break;
    case LSOF_PROTOCOL_HOPOPTS:
        (void)snpf(buf, buf_len, "HOPOPTS");
        break;
    case LSOF_PROTOCOL_ICMP:
        (void)snpf(buf, buf_len, "ICMP");
        break;
    case LSOF_PROTOCOL_IGMP:
        (void)snpf(buf, buf_len, "IGMP");
        break;
    case LSOF_PROTOCOL_GGP:
        (void)snpf(buf, buf_len, "GGP");
        break;
    case LSOF_PROTOCOL_IPIP:
        (void)snpf(buf, buf_len, "IPIP");
        break;
    case LSOF_PROTOCOL_IPV4:
        (void)snpf(buf, buf_len, "IPV4");
        break;
    case LSOF_PROTOCOL_TCP:
        (void)snpf(buf, buf_len, "TCP");
        break;
    case LSOF_PROTOCOL_ST:
        (void)snpf(buf, buf_len, "ST");
        break;
    case LSOF_PROTOCOL_EGP:
        (void)snpf(buf, buf_len, "EGP");
        break;
    case LSOF_PROTOCOL_PIGP:
        (void)snpf(buf, buf_len, "PIGP");
        break;
    case LSOF_PROTOCOL_RCCMON:
        (void)snpf(buf, buf_len, "RCCMON");
        break;
    case LSOF_PROTOCOL_NVPII:
        (void)snpf(buf, buf_len, "NVPII");
        break;
    case LSOF_PROTOCOL_PUP:
        (void)snpf(buf, buf_len, "PUP");
        break;
    case LSOF_PROTOCOL_ARGUS:
        (void)snpf(buf, buf_len, "ARGUS");
        break;
    case LSOF_PROTOCOL_EMCON:
        (void)snpf(buf, buf_len, "EMCON");
        break;
    case LSOF_PROTOCOL_XNET:
        (void)snpf(buf, buf_len, "XNET");
        break;
    case LSOF_PROTOCOL_CHAOS:
        (void)snpf(buf, buf_len, "CHAOS");
        break;
    case LSOF_PROTOCOL_UDP:
        (void)snpf(buf, buf_len, "UDP");
        break;
    case LSOF_PROTOCOL_MUX:
        (void)snpf(buf, buf_len, "MUX");
        break;
    case LSOF_PROTOCOL_MEAS:
        (void)snpf(buf, buf_len, "MEAS");
        break;
    case LSOF_PROTOCOL_HMP:
        (void)snpf(buf, buf_len, "HMP");
        break;
    case LSOF_PROTOCOL_PRM:
        (void)snpf(buf, buf_len, "PRM");
        break;
    case LSOF_PROTOCOL_IDP:
        (void)snpf(buf, buf_len, "IDP");
        break;
    case LSOF_PROTOCOL_TRUNK1:
        (void)snpf(buf, buf_len, "TRUNK1");
        break;
    case LSOF_PROTOCOL_TRUNK2:
        (void)snpf(buf, buf_len, "TRUNK2");
        break;
    case LSOF_PROTOCOL_LEAF1:
        (void)snpf(buf, buf_len, "LEAF1");
        break;
    case LSOF_PROTOCOL_LEAF2:
        (void)snpf(buf, buf_len, "LEAF2");
        break;
    case LSOF_PROTOCOL_RDP:
        (void)snpf(buf, buf_len, "RDP");
        break;
    case LSOF_PROTOCOL_IRTP:
        (void)snpf(buf, buf_len, "IRTP");
        break;
    case LSOF_PROTOCOL_TP:
        (void)snpf(buf, buf_len, "TP");
        break;
    case LSOF_PROTOCOL_BLT:
        (void)snpf(buf, buf_len, "BLT");
        break;
    case LSOF_PROTOCOL_NSP:
        (void)snpf(buf, buf_len, "NSP");
        break;
    case LSOF_PROTOCOL_INP:
        (void)snpf(buf, buf_len, "INP");
        break;
    case LSOF_PROTOCOL_DCCP:
        (void)snpf(buf, buf_len, "DCCP");
        break;
    case LSOF_PROTOCOL_SEP:
        (void)snpf(buf, buf_len, "SEP");
        break;
    case LSOF_PROTOCOL_3PC:
        (void)snpf(buf, buf_len, "3PC");
        break;
    case LSOF_PROTOCOL_IDPR:
        (void)snpf(buf, buf_len, "IDPR");
        break;
    case LSOF_PROTOCOL_XTP:
        (void)snpf(buf, buf_len, "XTP");
        break;
    case LSOF_PROTOCOL_DDP:
        (void)snpf(buf, buf_len, "DDP");
        break;
    case LSOF_PROTOCOL_CMTP:
        (void)snpf(buf, buf_len, "CMTP");
        break;
    case LSOF_PROTOCOL_TPXX:
        (void)snpf(buf, buf_len, "TPXX");
        break;
    case LSOF_PROTOCOL_IL:
        (void)snpf(buf, buf_len, "IL");
        break;
    case LSOF_PROTOCOL_IPV6:
        (void)snpf(buf, buf_len, "IPV6");
        break;
    case LSOF_PROTOCOL_SDRP:
        (void)snpf(buf, buf_len, "SDRP");
        break;
    case LSOF_PROTOCOL_ROUTING:
        (void)snpf(buf, buf_len, "ROUTING");
        break;
    case LSOF_PROTOCOL_FRAGMENT:
        (void)snpf(buf, buf_len, "FRAGMNT");
        break;
    case LSOF_PROTOCOL_IDRP:
        (void)snpf(buf, buf_len, "IDRP");
        break;
    case LSOF_PROTOCOL_RSVP:
        (void)snpf(buf, buf_len, "RSVP");
        break;
    case LSOF_PROTOCOL_GRE:
        (void)snpf(buf, buf_len, "GRE");
        break;
    case LSOF_PROTOCOL_MHRP:
        (void)snpf(buf, buf_len, "MHRP");
        break;
    case LSOF_PROTOCOL_BHA:
        (void)snpf(buf, buf_len, "BHA");
        break;
    case LSOF_PROTOCOL_ESP:
        (void)snpf(buf, buf_len, "ESP");
        break;
    case LSOF_PROTOCOL_AH:
        (void)snpf(buf, buf_len, "AH");
        break;
    case LSOF_PROTOCOL_INLSP:
        (void)snpf(buf, buf_len, "INLSP");
        break;
    case LSOF_PROTOCOL_SWIPE:
        (void)snpf(buf, buf_len, "SWIPE");
        break;
    case LSOF_PROTOCOL_NHRP:
        (void)snpf(buf, buf_len, "NHRP");
        break;
    case LSOF_PROTOCOL_MOBILE:
        (void)snpf(buf, buf_len, "MOBILE");
        break;
    case LSOF_PROTOCOL_TLSP:
        (void)snpf(buf, buf_len, "TLSP");
        break;
    case LSOF_PROTOCOL_SKIP:
        (void)snpf(buf, buf_len, "SKIP");
        break;
    case LSOF_PROTOCOL_ICMPV6:
        (void)snpf(buf, buf_len, "ICMPV6");
        break;
    case LSOF_PROTOCOL_NONE:
        (void)snpf(buf, buf_len, "NONE");
        break;
    case LSOF_PROTOCOL_DSTOPTS:
        (void)snpf(buf, buf_len, "DSTOPTS");
        break;
    case LSOF_PROTOCOL_AHIP:
        (void)snpf(buf, buf_len, "AHIP");
        break;
    case LSOF_PROTOCOL_CFTP:
        (void)snpf(buf, buf_len, "CFTP");
        break;
    case LSOF_PROTOCOL_HELLO:
        (void)snpf(buf, buf_len, "HELLO");
        break;
    case LSOF_PROTOCOL_SATEXPAK:
        (void)snpf(buf, buf_len, "STEXPAK");
        break;
    case LSOF_PROTOCOL_KRYPTOLAN:
        (void)snpf(buf, buf_len, "KRYPTLN");
        break;
    case LSOF_PROTOCOL_RVD:
        (void)snpf(buf, buf_len, "RVD");
        break;
    case LSOF_PROTOCOL_IPPC:
        (void)snpf(buf, buf_len, "IPPC");
        break;
    case LSOF_PROTOCOL_ADFS:
        (void)snpf(buf, buf_len, "ADFS");
        break;
    case LSOF_PROTOCOL_SATMON:
        (void)snpf(buf, buf_len, "SATMON");
        break;
    case LSOF_PROTOCOL_VISA:
        (void)snpf(buf, buf_len, "VISA");
        break;
    case LSOF_PROTOCOL_IPCV:
        (void)snpf(buf, buf_len, "IPCV");
        break;
    case LSOF_PROTOCOL_CPNX:
        (void)snpf(buf, buf_len, "CPNX");
        break;
    case LSOF_PROTOCOL_CPHB:
        (void)snpf(buf, buf_len, "CPHB");
        break;
    case LSOF_PROTOCOL_WSN:
        (void)snpf(buf, buf_len, "WSN");
        break;
    case LSOF_PROTOCOL_PVP:
        (void)snpf(buf, buf_len, "PVP");
        break;
    case LSOF_PROTOCOL_BRSATMON:
        (void)snpf(buf, buf_len, "BRSTMON");
        break;
    case LSOF_PROTOCOL_ND:
        (void)snpf(buf, buf_len, "ND");
        break;
    case LSOF_PROTOCOL_WBMON:
        (void)snpf(buf, buf_len, "WBMON");
        break;
    case LSOF_PROTOCOL_WBEXPAK:
        (void)snpf(buf, buf_len, "WBEXPAK");
        break;
    case LSOF_PROTOCOL_EON:
        (void)snpf(buf, buf_len, "EON");
        break;
    case LSOF_PROTOCOL_VMTP:
        (void)snpf(buf, buf_len, "VMTP");
        break;
    case LSOF_PROTOCOL_SVMTP:
        (void)snpf(buf, buf_len, "SVMTP");
        break;
    case LSOF_PROTOCOL_VINES:
        (void)snpf(buf, buf_len, "VINES");
        break;
    case LSOF_PROTOCOL_TTP:
        (void)snpf(buf, buf_len, "TTP");
        break;
    case LSOF_PROTOCOL_IGP:
        (void)snpf(buf, buf_len, "IGP");
        break;
    case LSOF_PROTOCOL_DGP:
        (void)snpf(buf, buf_len, "DGP");
        break;
    case LSOF_PROTOCOL_TCF:
        (void)snpf(buf, buf_len, "TCF");
        break;
    case LSOF_PROTOCOL_IGRP:
        (void)snpf(buf, buf_len, "IGRP");
        break;
    case LSOF_PROTOCOL_OSPFIGP:
        (void)snpf(buf, buf_len, "OSPFIGP");
        break;
    case LSOF_PROTOCOL_SRPC:
        (void)snpf(buf, buf_len, "SRPC");
        break;
    case LSOF_PROTOCOL_LARP:
        (void)snpf(buf, buf_len, "LARP");
        break;
    case LSOF_PROTOCOL_MTP:
        (void)snpf(buf, buf_len, "MTP");
        break;
    case LSOF_PROTOCOL_AX25:
        (void)snpf(buf, buf_len, "AX25");
        break;
    case LSOF_PROTOCOL_BEETPH:
        (void)snpf(buf, buf_len, "BEETPH");
        break;
    case LSOF_PROTOCOL_IPEIP:
        (void)snpf(buf, buf_len, "IPEIP");
        break;
    case LSOF_PROTOCOL_MICP:
        (void)snpf(buf, buf_len, "MICP");
        break;
    case LSOF_PROTOCOL_SCCSP:
        (void)snpf(buf, buf_len, "SCCSP");
        break;
    case LSOF_PROTOCOL_ETHERIP:
        (void)snpf(buf, buf_len, "ETHERIP");
        break;
    case LSOF_PROTOCOL_ENCAP:
        (void)snpf(buf, buf_len, "ENCAP");
        break;
    case LSOF_PROTOCOL_APES:
        (void)snpf(buf, buf_len, "APES");
        break;
    case LSOF_PROTOCOL_GMTP:
        (void)snpf(buf, buf_len, "GMTP");
        break;
    case LSOF_PROTOCOL_PIM:
        (void)snpf(buf, buf_len, "PIM");
        break;
    case LSOF_PROTOCOL_COMP:
        (void)snpf(buf, buf_len, "COMP");
        break;
    case LSOF_PROTOCOL_IPCOMP:
        (void)snpf(buf, buf_len, "IPCOMP");
        break;
    case LSOF_PROTOCOL_CARP:
        (void)snpf(buf, buf_len, "CARP");
        break;
    case LSOF_PROTOCOL_PGM:
        (void)snpf(buf, buf_len, "PGM");
        break;
    case LSOF_PROTOCOL_SCTP:
        (void)snpf(buf, buf_len, "SCTP");
        break;
    case LSOF_PROTOCOL_MH:
        (void)snpf(buf, buf_len, "MH");
        break;
    case LSOF_PROTOCOL_UDPLITE:
        (void)snpf(buf, buf_len, "UDPLITE");
        break;
    case LSOF_PROTOCOL_MPLS:
        (void)snpf(buf, buf_len, "MPLS");
        break;
    case LSOF_PROTOCOL_HIP:
        (void)snpf(buf, buf_len, "HIP");
        break;
    case LSOF_PROTOCOL_SHIM6:
        (void)snpf(buf, buf_len, "SHIM6");
        break;
    case LSOF_PROTOCOL_ETHERNET:
        (void)snpf(buf, buf_len, "ETHER");
        break;
    case LSOF_PROTOCOL_PFSYNC:
        (void)snpf(buf, buf_len, "PFSYNC");
        break;
    case LSOF_PROTOCOL_RESERVED_253:
        (void)snpf(buf, buf_len, "RSVD253");
        break;
    case LSOF_PROTOCOL_RESERVED_254:
        (void)snpf(buf, buf_len, "RSVD254");
        break;
    case LSOF_PROTOCOL_OLD_DIVERT:
        (void)snpf(buf, buf_len, "ODIVERT");
        break;
    case LSOF_PROTOCOL_RAW:
        (void)snpf(buf, buf_len, "RAW");
        break;
    case LSOF_PROTOCOL_MAX:
        (void)snpf(buf, buf_len, "MAX");
        break;
    case LSOF_PROTOCOL_DONE:
        (void)snpf(buf, buf_len, "DONE");
        break;
    case LSOF_PROTOCOL_SEND:
        (void)snpf(buf, buf_len, "SEND");
        break;
    case LSOF_PROTOCOL_MPTCP:
        (void)snpf(buf, buf_len, "MPTCP");
        break;
    case LSOF_PROTOCOL_SPACER:
        (void)snpf(buf, buf_len, "SPACER");
        break;
    case LSOF_PROTOCOL_802_3:
        (void)snpf(buf, buf_len, "802_3");
        break;
    case LSOF_PROTOCOL_ALL:
        (void)snpf(buf, buf_len, "ALL");
        break;
    case LSOF_PROTOCOL_802_2:
        (void)snpf(buf, buf_len, "802_2");
        break;
    case LSOF_PROTOCOL_SNAP:
        (void)snpf(buf, buf_len, "SNAP");
        break;
    case LSOF_PROTOCOL_DDCMP:
        (void)snpf(buf, buf_len, "DDCMP");
        break;
    case LSOF_PROTOCOL_WAN_PPP:
        (void)snpf(buf, buf_len, "WAN_PPP");
        break;
    case LSOF_PROTOCOL_PPP_MP:
        (void)snpf(buf, buf_len, "PPP_MP");
        break;
    case LSOF_PROTOCOL_LOCALTALK:
        (void)snpf(buf, buf_len, "LCLTALK");
        break;
    case LSOF_PROTOCOL_CAN:
        (void)snpf(buf, buf_len, "CAN");
        break;
    case LSOF_PROTOCOL_CANFD:
        (void)snpf(buf, buf_len, "CANFD");
        break;
    case LSOF_PROTOCOL_CANXL:
        (void)snpf(buf, buf_len, "CANXL");
        break;
    case LSOF_PROTOCOL_PPPTALK:
        (void)snpf(buf, buf_len, "PPPTALK");
        break;
    case LSOF_PROTOCOL_TR_802_2:
        (void)snpf(buf, buf_len, "802.2");
        break;
    case LSOF_PROTOCOL_MOBITEX:
        (void)snpf(buf, buf_len, "MOBITEX");
        break;
    case LSOF_PROTOCOL_CONTROL:
        (void)snpf(buf, buf_len, "CONTROL");
        break;
    case LSOF_PROTOCOL_IRDA:
        (void)snpf(buf, buf_len, "IRDA");
        break;
    case LSOF_PROTOCOL_ECONET:
        (void)snpf(buf, buf_len, "ECONET");
        break;
    case LSOF_PROTOCOL_HDLC:
        (void)snpf(buf, buf_len, "HDLC");
        break;
    case LSOF_PROTOCOL_ARCNET:
        (void)snpf(buf, buf_len, "ARCNET");
        break;
    case LSOF_PROTOCOL_DSA:
        (void)snpf(buf, buf_len, "DSA");
        break;
    case LSOF_PROTOCOL_TRAILER:
        (void)snpf(buf, buf_len, "TRAILER");
        break;
    case LSOF_PROTOCOL_LOOP:
        (void)snpf(buf, buf_len, "LOOP");
        break;
    case LSOF_PROTOCOL_PHONET:
        (void)snpf(buf, buf_len, "PHONET");
        break;
    case LSOF_PROTOCOL_IEEE802154:
        (void)snpf(buf, buf_len, "802154");
        break;
    case LSOF_PROTOCOL_CAIF:
        (void)snpf(buf, buf_len, "CAIF");
        break;
    case LSOF_PROTOCOL_XDSA:
        (void)snpf(buf, buf_len, "XDSA");
        break;
    case LSOF_PROTOCOL_MAP:
        (void)snpf(buf, buf_len, "MAP");
        break;
    case LSOF_PROTOCOL_MCTP:
        (void)snpf(buf, buf_len, "MCTP");
        break;
    case LSOF_PROTOCOL_PUPAT:
        (void)snpf(buf, buf_len, "PUPAT");
        break;
    case LSOF_PROTOCOL_802_3_MIN:
        (void)snpf(buf, buf_len, "802.3");
        break;
    case LSOF_PROTOCOL_X25:
        (void)snpf(buf, buf_len, "X25");
        break;
    case LSOF_PROTOCOL_ARP:
        (void)snpf(buf, buf_len, "ARP");
        break;
    case LSOF_PROTOCOL_BPQ:
        (void)snpf(buf, buf_len, "BPQ");
        break;
    case LSOF_PROTOCOL_IEEEPUP:
        (void)snpf(buf, buf_len, "IEEEPUP");
        break;
    case LSOF_PROTOCOL_IEEEPUPAT:
        (void)snpf(buf, buf_len, "I3EPUPAT");
        break;
    case LSOF_PROTOCOL_ERSPAN2:
        (void)snpf(buf, buf_len, "ERSPAN2");
        break;
    case LSOF_PROTOCOL_TSN:
        (void)snpf(buf, buf_len, "TSN");
        break;
    case LSOF_PROTOCOL_BATMAN:
        (void)snpf(buf, buf_len, "BATMAN");
        break;
    case LSOF_PROTOCOL_DEC:
        (void)snpf(buf, buf_len, "DEC");
        break;
    case LSOF_PROTOCOL_DNA_DL:
        (void)snpf(buf, buf_len, "DNA_DL");
        break;
    case LSOF_PROTOCOL_DNA_RC:
        (void)snpf(buf, buf_len, "DNA_RC");
        break;
    case LSOF_PROTOCOL_DNA_RT:
        (void)snpf(buf, buf_len, "DNA_RT");
        break;
    case LSOF_PROTOCOL_LAT:
        (void)snpf(buf, buf_len, "LAT");
        break;
    case LSOF_PROTOCOL_DIAG:
        (void)snpf(buf, buf_len, "DIAG");
        break;
    case LSOF_PROTOCOL_CUST:
        (void)snpf(buf, buf_len, "CUST");
        break;
    case LSOF_PROTOCOL_SCA:
        (void)snpf(buf, buf_len, "SCA");
        break;
    case LSOF_PROTOCOL_TEB:
        (void)snpf(buf, buf_len, "TEB");
        break;
    case LSOF_PROTOCOL_RARP:
        (void)snpf(buf, buf_len, "RARP");
        break;
    case LSOF_PROTOCOL_ATALK:
        (void)snpf(buf, buf_len, "ATALK");
        break;
    case LSOF_PROTOCOL_AARP:
        (void)snpf(buf, buf_len, "AARP");
        break;
    case LSOF_PROTOCOL_8021Q:
        (void)snpf(buf, buf_len, "8021Q");
        break;
    case LSOF_PROTOCOL_IPX:
        (void)snpf(buf, buf_len, "IPX");
        break;
    case LSOF_PROTOCOL_PAUSE:
        (void)snpf(buf, buf_len, "PAUSE");
        break;
    case LSOF_PROTOCOL_SLOW:
        (void)snpf(buf, buf_len, "SLOW");
        break;
    case LSOF_PROTOCOL_WCCP:
        (void)snpf(buf, buf_len, "WCCP");
        break;
    case LSOF_PROTOCOL_MPLS_UC:
        (void)snpf(buf, buf_len, "MPLS_UC");
        break;
    case LSOF_PROTOCOL_MPLS_MC:
        (void)snpf(buf, buf_len, "MPLS_MC");
        break;
    case LSOF_PROTOCOL_ATMMPOA:
        (void)snpf(buf, buf_len, "ATMMPOA");
        break;
    case LSOF_PROTOCOL_PPP_DISC:
        (void)snpf(buf, buf_len, "PPPDISC");
        break;
    case LSOF_PROTOCOL_PPP_SES:
        (void)snpf(buf, buf_len, "PPP_SES");
        break;
    case LSOF_PROTOCOL_LINK_CTL:
        (void)snpf(buf, buf_len, "LINKCTL");
        break;
    case LSOF_PROTOCOL_ATMFATE:
        (void)snpf(buf, buf_len, "ATMFATE");
        break;
    case LSOF_PROTOCOL_PAE:
        (void)snpf(buf, buf_len, "PAE");
        break;
    case LSOF_PROTOCOL_PROFINET:
        (void)snpf(buf, buf_len, "PROFINT");
        break;
    case LSOF_PROTOCOL_REALTEK:
        (void)snpf(buf, buf_len, "REALTEK");
        break;
    case LSOF_PROTOCOL_AOE:
        (void)snpf(buf, buf_len, "AOE");
        break;
    case LSOF_PROTOCOL_ETHERCAT:
        (void)snpf(buf, buf_len, "ETHCAT");
        break;
    case LSOF_PROTOCOL_8021AD:
        (void)snpf(buf, buf_len, "8021AD");
        break;
    case LSOF_PROTOCOL_802_EX1:
        (void)snpf(buf, buf_len, "802_EX1");
        break;
    case LSOF_PROTOCOL_ERSPAN:
        (void)snpf(buf, buf_len, "ERSPAN");
        break;
    case LSOF_PROTOCOL_PREAUTH:
        (void)snpf(buf, buf_len, "PREAUTH");
        break;
    case LSOF_PROTOCOL_TIPC:
        (void)snpf(buf, buf_len, "TIPC");
        break;
    case LSOF_PROTOCOL_LLDP:
        (void)snpf(buf, buf_len, "LLDP");
        break;
    case LSOF_PROTOCOL_MRP:
        (void)snpf(buf, buf_len, "MRP");
        break;
    case LSOF_PROTOCOL_MACSEC:
        (void)snpf(buf, buf_len, "MACSEC");
        break;
    case LSOF_PROTOCOL_8021AH:
        (void)snpf(buf, buf_len, "8021AH");
        break;
    case LSOF_PROTOCOL_MVRP:
        (void)snpf(buf, buf_len, "MVRP");
        break;
    case LSOF_PROTOCOL_1588:
        (void)snpf(buf, buf_len, "1588");
        break;
    case LSOF_PROTOCOL_NCSI:
        (void)snpf(buf, buf_len, "NCSI");
        break;
    case LSOF_PROTOCOL_PRP:
        (void)snpf(buf, buf_len, "PRP");
        break;
    case LSOF_PROTOCOL_CFM:
        (void)snpf(buf, buf_len, "CFM");
        break;
    case LSOF_PROTOCOL_FCOE:
        (void)snpf(buf, buf_len, "FCOE");
        break;
    case LSOF_PROTOCOL_TDLS:
        (void)snpf(buf, buf_len, "TDLS");
        break;
    case LSOF_PROTOCOL_FIP:
        (void)snpf(buf, buf_len, "FIP");
        break;
    case LSOF_PROTOCOL_IBOE:
        (void)snpf(buf, buf_len, "IBOE");
        break;
    case LSOF_PROTOCOL_80221:
        (void)snpf(buf, buf_len, "80221");
        break;
    case LSOF_PROTOCOL_HSR:
        (void)snpf(buf, buf_len, "HSR");
        break;
    case LSOF_PROTOCOL_NSH:
        (void)snpf(buf, buf_len, "NSH");
        break;
    case LSOF_PROTOCOL_LOOPBACK:
        (void)snpf(buf, buf_len, "LOOPBAK");
        break;
    case LSOF_PROTOCOL_QINQ1:
        (void)snpf(buf, buf_len, "QINQ1");
        break;
    case LSOF_PROTOCOL_QINQ2:
        (void)snpf(buf, buf_len, "QINQ2");
        break;
    case LSOF_PROTOCOL_QINQ3:
        (void)snpf(buf, buf_len, "QINQ3");
        break;
    case LSOF_PROTOCOL_EDSA:
        (void)snpf(buf, buf_len, "EDSA");
        break;
    case LSOF_PROTOCOL_DSA_8021Q:
        (void)snpf(buf, buf_len, "DSAD1Q");
        break;
    case LSOF_PROTOCOL_DSA_A5PSW:
        (void)snpf(buf, buf_len, "DSA5PSW");
        break;
    case LSOF_PROTOCOL_IFE:
        (void)snpf(buf, buf_len, "IFE");
        break;
    case LSOF_PROTOCOL_AF_IUCV:
        (void)snpf(buf, buf_len, "AF_IUCV");
        break;
    }
}
