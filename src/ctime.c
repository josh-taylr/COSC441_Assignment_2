/*  File   : ctime.c
    Author : Richard A. O'Keefe
    Updated: %G%
    Purpose: Measure how loong a command takes, more precisely than time(1).
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct tms before, after;
    pid_t pid;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Usage: ctime cmd...\n");
        return EXIT_FAILURE;
    }
    (void)times(&before);
    pid = fork();
    if (pid == 0) {
	/* I am the child */
        (void)execvp(argv[1], argv+1);
        perror(argv[1]);
        return EXIT_FAILURE;
    }
    if (pid == (pid_t)(-1)) {
        /* The fork() didn't work */
	perror(argv[1]);
	return EXIT_FAILURE;
    }
    /* I am the parent */
    waitpid(pid, &status, 0);
    (void)times(&after);
    {
#ifndef CLK_TCK
	clock_t CLK_TCK = sysconf(_SC_CLK_TCK);
#endif
	double u = (double)after.tms_cutime - (double)before.tms_cutime;
	double s = (double)after.tms_cstime - (double)before.tms_cstime;
	u /= CLK_TCK;
	s /= CLK_TCK;
        fprintf(stderr, "%.3f user + %.3f system = %.3f total seconds.\n",
	        u, s, u+s);
    }
    return EXIT_SUCCESS;
}

