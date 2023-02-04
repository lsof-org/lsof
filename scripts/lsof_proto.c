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
    case LSOF_PROTOCOL_IPIP:
        (void)snpf(buf, buf_len, "IPIP");
        break;
    case LSOF_PROTOCOL_TCP:
        (void)snpf(buf, buf_len, "TCP");
        break;
    case LSOF_PROTOCOL_EGP:
        (void)snpf(buf, buf_len, "EGP");
        break;
    case LSOF_PROTOCOL_PUP:
        (void)snpf(buf, buf_len, "PUP");
        break;
    case LSOF_PROTOCOL_UDP:
        (void)snpf(buf, buf_len, "UDP");
        break;
    case LSOF_PROTOCOL_IDP:
        (void)snpf(buf, buf_len, "IDP");
        break;
    case LSOF_PROTOCOL_TP:
        (void)snpf(buf, buf_len, "TP");
        break;
    case LSOF_PROTOCOL_DCCP:
        (void)snpf(buf, buf_len, "DCCP");
        break;
    case LSOF_PROTOCOL_IPV6:
        (void)snpf(buf, buf_len, "IPV6");
        break;
    case LSOF_PROTOCOL_ROUTING:
        (void)snpf(buf, buf_len, "ROUTING");
        break;
    case LSOF_PROTOCOL_FRAGMENT:
        (void)snpf(buf, buf_len, "FRAGMNT");
        break;
    case LSOF_PROTOCOL_RSVP:
        (void)snpf(buf, buf_len, "RSVP");
        break;
    case LSOF_PROTOCOL_GRE:
        (void)snpf(buf, buf_len, "GRE");
        break;
    case LSOF_PROTOCOL_ESP:
        (void)snpf(buf, buf_len, "ESP");
        break;
    case LSOF_PROTOCOL_AH:
        (void)snpf(buf, buf_len, "AH");
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
    case LSOF_PROTOCOL_MTP:
        (void)snpf(buf, buf_len, "MTP");
        break;
    case LSOF_PROTOCOL_BEETPH:
        (void)snpf(buf, buf_len, "BEETPH");
        break;
    case LSOF_PROTOCOL_ENCAP:
        (void)snpf(buf, buf_len, "ENCAP");
        break;
    case LSOF_PROTOCOL_PIM:
        (void)snpf(buf, buf_len, "PIM");
        break;
    case LSOF_PROTOCOL_COMP:
        (void)snpf(buf, buf_len, "COMP");
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
    case LSOF_PROTOCOL_ETHERNET:
        (void)snpf(buf, buf_len, "ETHER");
        break;
    case LSOF_PROTOCOL_RAW:
        (void)snpf(buf, buf_len, "RAW");
        break;
    case LSOF_PROTOCOL_MPTCP:
        (void)snpf(buf, buf_len, "MPTCP");
        break;
    case LSOF_PROTOCOL_802_3:
        (void)snpf(buf, buf_len, "802_3");
        break;
    case LSOF_PROTOCOL_AX25:
        (void)snpf(buf, buf_len, "AX25");
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
