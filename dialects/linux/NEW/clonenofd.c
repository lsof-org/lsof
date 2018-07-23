#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/unistd.h>
#define _GNU_SOURCE
#include <sched.h>


#define THREADS 2
#define NAMELEN 32
#define STACKSZ 64*1024

int opentmpfile(int i) {
	char fname[NAMELEN];

	sprintf(fname, "/tmp/foo.%i.XXXXXX", i);
	return mkstemp(fname);
}


int thread_function (void *arg) {
	int fd, i = (int) arg;

	fd = opentmpfile(i);
	lseek(fd, i, SEEK_SET);
	sleep(100);

	return 0;
}

int main () {
	int i, pid, tid[THREADS];
	char *stack;

	pid = getpid();
	printf("I'm PID %i\n", pid);

	opentmpfile(0);

	printf("clone threads: \n");
	stack = malloc(STACKSZ * THREADS); 
	for(i = 0; i < THREADS; i++) {
		tid[i] = clone(&thread_function, stack + STACKSZ * (i+1),
			       SIGCHLD | CLONE_THREAD | CLONE_SIGHAND | CLONE_VM,
			       (void*) (i+1) );
		if(tid[i] == -1)
			perror("clone failed");
		printf("tid[%i] = %i\n", i, tid[i]);
	}
	getchar();

	for(i = 0; i < THREADS; i++)
		syscall(__NR_tgkill, pid, tid[i], SIGKILL);
	
	return 0;
}

