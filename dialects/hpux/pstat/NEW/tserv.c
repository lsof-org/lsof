#include <stdio.h>
#include <signal.h>
#include <stropts.h>
#include <sys/tiuser.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TCP_PATH   "/dev/tcp"
#define DISCONNECT -1
#define TCP_UNIQUE 5001

  extern int  t_errno;
  void        run_server(int);
  int         accept_call(int, struct t_call *);
  void        connrelease();
  int         conn_fd;
  char        *netpath  = TCP_PATH;

  main(int argc, char *argv[])
  {
       int listen_fd;
       struct t_bind *bind;
       struct t_call *call;
       struct sockaddr_in srv_addr;

       if ((listen_fd =
                  t_open(netpath, O_RDWR, (struct t_info *) NULL)) < 0) {
           t_error("t_open failed for listen_fd");
           exit(1);
       }

       if ((bind = (struct t_bind *)t_alloc(listen_fd, T_BIND, T_ADDR)) == NULL) {
           t_error("t_alloc of t_bind structure failed");
           exit(2);
       }
       bind->qlen = 5;
       bind->addr.len = sizeof(srv_addr);
       bind->addr.buf = (char *)&srv_addr;

       srv_addr.sin_family = AF_INET;
       srv_addr.sin_addr.s_addr = INADDR_ANY;
       srv_addr.sin_port = TCP_UNIQUE;

       if (t_bind(listen_fd, bind, bind) < 0) {
           t_error("t_bind failed for listen_fd");
           exit(3);
       }

       if ((call = (struct t_call *)t_alloc(listen_fd, T_CALL, T_ADDR)) == NULL) {
           t_error("t_alloc failed");
           exit(5);
       }
       call->addr.len = sizeof(srv_addr);
       call->udata.maxlen = 0;

       while (1) {
           if (t_listen(listen_fd, call) < 0) {
               t_error("t_listen failed for listen_fd");
               exit(6);
           }
           if ((conn_fd = accept_call(listen_fd, call)) != DISCONNECT)
               run_server(listen_fd);
       }
  }

  int
  accept_call(int listen_fd, struct t_call *call)
  {
       int resfd;

       if ((resfd = t_open(netpath, O_RDWR, (struct t_info *) NULL))
                                                                < 0) {
           t_error("t_open for responding fd failed");
           exit(7);
       }

       if (t_bind(resfd, (struct t_bind *) NULL,
                         (struct t_bind *) NULL) < 0) {
           t_error("t_bind for responding fd failed");
           exit(8);
       }

       if (t_accept(listen_fd, resfd, call) < 0) {
           if (t_errno == TLOOK) {
               if (t_rcvdis(listen_fd, (struct t_discon *) NULL) < 0) {
                   t_error("t_rcvdis failed for listen_fd");
                   exit(9);
               }
               if (t_close(resfd) < 0) {
                   t_error("t_close failed for responding fd");
                   exit(10);
               }
               return(DISCONNECT);
           }
           t_error("t_accept failed");
           exit(11);
       }

       return(resfd);
  }

  void
  connrelease()
  {
       exit(0);
  }

  void
  run_server(int listen_fd)
  {
       int nbytes;
      FILE *logfp;

      struct {
          unsigned short  length;
          char            buf[1024];
      } msg;

      switch (fork()) {

      case -1:
          perror("fork failed");
          exit(12);

      default:                   /* parent */
          /* close conn_fd and go up to listen again */
          if (t_close(conn_fd) < 0) {
              t_error("t_close failed for conn_fd");
              exit(13);
          }
          return;

      case 0:                    /* child */
          /* close listen_fd and do service */
          if (t_close(listen_fd) < 0) {
              t_error("t_close failed for listen_fd");
              exit(14);
          }
          if ((logfp = fopen("logfile", "r")) == NULL) {
              perror("cannot open logfile");
              exit(15);
          }
          signal(SIGPOLL, connrelease);

          if (ioctl(conn_fd, I_SETSIG, S_INPUT) < 0) {
              perror("ioctl I_SETSIG failed");
              exit(16);
          }

          if (t_look(conn_fd) != 0) {
              fprintf(stderr, "t_look returned unexpected event");
              exit(17);
          }

          while ((nbytes = fread(msg.buf, 1, 1024, logfp)) > 0) {
              msg.length = nbytes;
              if (t_snd(conn_fd, &msg, nbytes +
                                  sizeof(msg.length), 0) < 0) {
                  t_error("t_snd failed");
                  exit(18);
              }
          }

          msg.length = 0;

          if (t_snd(conn_fd, &msg, 2, 0) <  0)  {
              t_error("can't send 0 bytes");
              exit(19);
          }

          pause();  /* until disconnect indication arrives */
      }
 }


