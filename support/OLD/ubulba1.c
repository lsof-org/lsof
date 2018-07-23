/*	ubulb.c		850921		*/

/*	Ubulb tests sockets: stream connected sockets in the Unix
 *	domain.
 *
 *	Ubulb does its job by providing a complete client/server pair in
 *	one invocation.  Ubulb may also be invoked separately to provide
 *	a separate client and a separate server.
 *
 *	After the client and server have established a connection, they
 *	exchange data.  The client accepts data from its standard input
 *	stream and sends it to the server, which echoes the data to its
 *	standard output stream.  The client prompts for input with a
 *	prompt string, "Server < ", received from the server; the
 *	server identifies data received from the client with the prefix
 *	"Client > ".
 *
 *	The connection may be broken by terminating the client or by
 *	supplying an EOF (^D) to its standard input.
 *
 *	V. Abell, Purdue University Computing Center
 */

/*	Contains:
 *
 *	main()		- the main program
 *
 *	Exit()		- clean up and exit
 *
 *	Perror()	- call perror() and exit
 *
 *	startclient()	- start a client
 *
 *	usage()		- report usage and exit
 */

/*	Usage:
 *
 *	bulb [-c] [-n name] [-s]
 *
 *	-c		run only the client
 *
 *	-n name		Unix domain socket name
 *			Default Unix_domain_socket
 *
 *	-s		run only the server
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>


/*
 * Externals
 */

int	 errno;			/* error number from perror.c */


/*
 * Local incantations
 */

#define CPREFIX		"Client > "	/* client data prefix */
#define	MXLL		256		/* maximum line length */
#define SPROMPT		"Server %d < "	/* server prompt */
#define UNIXSNM		"Unix_domain_socket"
					/* default Unix domain socket name */


/*
 * Globals
 */

int Asock;			/* accept socket address */
int Csock;			/* connect socket address */
int Lsock;			/* listen socket FD */
struct sockaddr_un Ua;		/* Unix socket address */
char *Uname = UNIXSNM;		/* Unix socket name */


void
SetNodelay(fd)
     int fd;
{
	int flags;

	if ((flags = fcntl(fd, F_GETFL, 0)) >= 0)
	{
        flags |= O_NDELAY;
        (void) fcntl(fd, F_SETFL, flags);
	}
}

void
prompt (fd)
     int fd;
{
    char buf[80];
    int len;
    
    len = sprintf (buf, SPROMPT, fd);
    if (write (fd, buf, len) != len)
        Perror("Server write()");
}

/*
 * main(argc, argv) - the main function
 */

main(argc, argv)
		int	 argc;
		char	*argv[];
{
		char	*a;			/* argument pointer */
struct	sockaddr_un	 aad;			/* accept address */
		int	 ax;			/* argument index */
		char	 buf[MXLL];		/* line buffer */
		int	 client = 1;		/* client status */
register	int	 len;			/* length */
		int	 pid;			/* child process ID */
		int	 nl;			/* name length */
register	int	 pl;			/* prompt length */
		int	 server = 1;		/* server status */

        /* for the select call...*/
        int  nfds;
        fd_set rfds;
/*
 * Process arguments.
 */

	if (argc > 1) {
		for (ax = 1; ax < argc; ax++) {
			a = argv[ax];
			if (*a != '-')
				usage();
			switch (*(a+1)) {

			case 'c':
				client = 1;
				server = 0;
				break;

			case 'n':
				if (++ax >= argc)
					usage();
				Uname = argv[ax];
				break;

			case 's':
				client = 0;
				server = 1;
				break;
			}
		}
	}
/*
 * Get a socket for the server and bind the local address.
 *
 * Then listen for a connection.
 */
	(void) memset((char *)&Ua, 0, sizeof(Ua));
	Ua.sun_family = AF_UNIX;
	(void) strcpy(Ua.sun_path, Uname);
	if (server) {
		(void) unlink(Uname);
		if ((Lsock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
			Perror("socket");
		if (bind(Lsock, (struct sockaddr *)&Ua, sizeof(Ua)) < 0)
			Perror("bind");
		if (chmod(Ua.sun_path, 0644) != 0)
			Perror("chmod");
		if (listen(Lsock, 1) != 0)
			Perror("Server listen");
        printf ("Listen socket is %d, %s\n", Lsock, Uname);
	}
/*
 * Start a child to execute the client, if necessary.
 */
	if (client) {
		if (server) {
			if ((pid = fork()) < 0)
				Perror("Server fork()");
			if (pid == 0) {
				startclient();
				(void) fprintf(stderr,
					"Client exited unexpectedly.");
				Exit(1);
			}
		} else
			startclient();
	}
    
/*
 * Accept a connection from a client.
 */
	(void) printf("The server is waiting for an incoming connection.\n");
	(void) fflush(stdout);

    nfds = Lsock + 1;
    FD_ZERO (&rfds);
    FD_SET (Lsock, &rfds);
    
    for (;;)
    {
        int ret;
        fd_set ret_fds;
        
        ret_fds = rfds;

        ret = select (nfds, &ret_fds, NULL, NULL, NULL);

        if (ret < 0)
        {
            if (errno != EINTR)
            {
                perror ("error on select()");
                Exit (0);
            }
        }
        
        if (ret <= 0) continue;

        if (Lsock && FD_ISSET (Lsock, &ret_fds))
        {
            /* accept the connection */
            nl = sizeof(aad);
            if ((Asock = accept(Lsock, (struct sockaddr *)&aad, &nl)) < 0)
                Perror("Server accept");
            (void) printf("Server: connection accepted with: \"%s\" - socket %d\n",
                          aad.sun_path, Asock);
            (void) fflush(stdout);

            /* set socket to no-delay */
            SetNodelay (Asock);

            /* send prompt to client */
            prompt (Asock);

            FD_SET (Asock, &rfds);
            if (nfds <= Asock)
                nfds = Asock + 1;

            ret--;
        }

        if (ret)  /* any more set? */
        {
            int f;
            for (f=0; ret && f < nfds; f++)
            {
                if (FD_ISSET (f, &ret_fds))
                {
                    if ((len = read(f, buf, sizeof(buf))) <= 0)
                    {
                        if (len < 0)
                            Perror("Server read()");
                        printf("Server: connection %d closed.\n", f);
                        close (f);
                        FD_CLR (f, &rfds);
                    }
                    else
                    {
                        buf[len] = '\0';
                        printf ("Client %d > %s", f, buf);
                        if (strrchr(buf, '\n') == NULL)
                            (void) printf("\n");
                        (void) fflush(stdout);
                        prompt (f);
                    }
                    ret--;
                }

            }
        }
    }                    
/* NOTREACHED */
}


/*
 * Exit(s)  - clean up and exit
 *
 * entry:
 *	Asock = server accept socket
 *	Csock = client socket
 *	Lsock = listen socket
 *	s     = exit status
 */

Exit(s)
		int	 s;
{
	if (Csock >= 0) {
		(void) shutdown(Csock, 2);
		(void) close(Csock);
	}
	if (Asock >= 0)
		(void) close(Asock);
	if (Lsock >= 0)
		(void) close(Lsock);
	exit(s);
}


/*
 * Perror(s) - call perror() and exit
 *
 * entry:
 *	s = pointer to perror() argument string
 */

Perror(s)
		char	*s;
{
	perror(s);
	Exit(1);
}


/*
 * startclient() - start a client
 */

startclient()
{
    struct	sockaddr_un	 bad;		/* bind address */
    char	 buf[MXLL];	/* line buffer */
    register	int	 len;		/* line length */
    fd_set fds;
    int ret;
/*
 * Create a connection socket
 */
	if ((Csock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		Perror("Connect socket");
	bad = Ua;
	len = strlen(bad.sun_path);
	(void) strcpy(&bad.sun_path[len], "_C");
	(void) unlink(bad.sun_path);
	if (bind(Csock, (struct sockaddr *)&bad, sizeof(bad)) < 0)
		Perror(bad.sun_path);
	if (chmod(bad.sun_path, 0644) != 0)
		Perror("chmod");
/*
 * Connect to the server in the remote peer.
 */
	if (connect(Csock, (struct sockaddr *)&Ua, sizeof(Ua)) != 0)
		Perror("Client connect()");
	(void) printf("Client connected! socket %d\n", Csock);
	(void) fflush(stdout);
    SetNodelay (Csock);
    
/*
 * After each prompt is received from the remote client,
 * send the next line from the standard input stream.
 */
	(void) clearerr(stdin);	/* DEBUG */

	for (errno = 0;;) {
        FD_ZERO (&fds);
        FD_SET (Csock, &fds);
        if ((ret = select (Csock+1, &fds, NULL, NULL, NULL)) < 0)
        {
            if (errno != EINTR)
            {
                perror ("error on select()");
                Exit (0);
            }
        }
        
        if (ret <= 0) continue;
        
		if (FD_ISSET (Csock, &fds) && 
            (len = read(Csock, buf, sizeof(buf))) <= 0) {
			if (errno != 0)
				Perror("Client read()");
			(void) fprintf(stderr, "Client: server sent EOF.\n");
			break;
		}
		if (len == sizeof(buf))
			buf[len-1] = '\0';
		else
			buf[len] = '\0';
		(void) printf("%s", buf);
		(void) fflush(stdout);
		if (fgets(buf, sizeof(buf), stdin) == NULL)
			break;
		len = strlen(buf);
		if (write(Csock, buf, len) < 0)
			Perror("Client write()");
		if (len == 0 || buf[len-1] != '\n') {
			if (write(Csock, "\n", 1) < 0)
				Perror("Client write()");
		}
	}
	(void) printf("^D\nClient: EOF received on stdin.\n");
	Exit(0);

/* NOTREACHED */

}




/*
 * usage() - report usage and exit
 */

usage()

{
	(void) fprintf(stderr,
	    "ubulb usage: [-c] [-n name] [-s]\n");
	Exit(1);
}
