#include <stdio.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int
main(int argc, char **argv)
{
  int epfd = epoll_create (1);
  if (epfd < 0)
    {
      perror ("epoll_create");
      return 1;
    }

  struct epoll_event ev;
  int fd[2];
  if (pipe(fd) < 0)
  if (fd < 0)
    {
      perror ("pipe");
      return 1;
    }
  if (dup2(fd[0], 5) < 0)
    {
      perror ("dup2(fd[0], 5)");
      return 1;
    }
  if (dup2(fd[1], 6) < 0)
    {
      perror ("dup2(fd[1], 6)");
      return 1;
    }

  ev.events = EPOLLOUT;
  ev.data.fd = 6;
  if (epoll_ctl (epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0)
    {
      perror ("epoll_ctl<6>");
      return 1;
    }

  ev.events = EPOLLIN;
  ev.data.fd = 5;
  if (epoll_ctl (epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0)
    {
      perror ("epoll_ctl<5>");
      return 1;
    }

  printf ("%d %d\n", getpid(), epfd);
  fflush (stdout);
  pause ();
  return 0;
}
