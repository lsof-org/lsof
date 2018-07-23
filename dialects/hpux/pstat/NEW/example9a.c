/*
 * Example 9A: Acquire protocol information of an XTI
 *             or a TLI TCP/UDP end-point.
 */
#include <stdio.h>
#include <sys/param.h>

#define	_PSTAT_STREAM_GET_XPORT	1

#include <sys/pstat.h>
#include <sys/unistd.h>
#include <sys/xti.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
int
main() {
    struct pst_fileinfo2 psf;
    struct pst_stream *psfstream;
    int ec, rv, count, fd, i;

    if ((fd = t_open("/dev/tcp", O_RDWR, NULL)) < 0 ) {
 	t_error("t_open() failed /dev/tcp");
	exit(-1);
    };

    if ((rv = t_bind(fd, NULL, NULL)) < 0 ) {
	t_error("t_bind() failed on the fd");
	exit(-1);
    };
    if ((rv = pstat_getfile2(&psf, sizeof(psf), 0, fd, getpid())) != 1)
	perror("pstat_getfile2");
    ec = psf.psf_nstrentt;
    psfstream = (struct pst_stream *)malloc(ec * sizeof(struct pst_stream));
    if (!psfstream)
	perror("malloc failed");
/*
 * Get Transport protocol information for this stream.
 */
    if ((count = pstat_getstream(psfstream, sizeof(struct pst_stream),
                                              ec, 0, &(psf.psf_fid)))
    > 0)
    {
	for (i = 0; i < ec; i++) {
	    if (psfstream[i].type == PS_STR_HEAD)
		break;
	}
	if (i >= ec) {
	    fprintf(stderr, "no stream head\n");
	    return(1);
	}
	if ((psfstream[i].val.head.pst_hi_fileid == psf.psf_hi_fileid)
	&&  (psfstream[i].val.head.pst_lo_fileid == psf.psf_lo_fileid)
	&&  (psfstream[i].val.head.pst_hi_nodeid == psf.psf_hi_nodeid)
	&&  (psfstream[i].val.head.pst_lo_nodeid == psf.psf_lo_nodeid))
	{
	    printf("Success\n");
	    printf("The major number of the stream is %d\n",
	        psfstream[i].val.head.pst_dev_major);
	    if (psfstream[i].pst_extn_flags & PS_STR_XPORT_DATA) {
		if (psfstream[i].pst_str_xport_protocol
		==  PS_STR_XPORT_TCP_PROTO)
		{
		    printf("TCP max hiwat %d\n",
			psfstream[i].pst_str_xport_obufsz);
		    printf("Local port number is %d\n",
		    ((struct sockaddr_in *)
			(psfstream[i].pst_str_xport_boundaddr)) ->sin_port
		    );
		    printf("TCP protocol state %d\n",
			psfstream[i].pst_str_xport_pstate);
		} else
		    printf("No TCP eXPORT data\n");
	    } else
		printf("No eXPORT data\n");
	} else
	    printf("State changed\n");
    } else
	perror("pstat_getstream()");
    return(0);
}
