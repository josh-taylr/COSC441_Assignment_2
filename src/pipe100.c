/* 
 * File:    pipe1.c 
 * Author:  Josh Taylor 
 * Created: 15 August, 2014 
 *
 * Uses the GNU pipe functions to send individual floating point number between 
 * threads.
 * 
 * Simple program to  to investigate the speed of passing information between 
 * two threads. The producer thread generates 1,000,000,000 random floating 
 * point numbers. The consumer thread receives 1,000,000,000 floating point 
 * number and summarises them.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <Math.h>
#include <string.h>
#include <errno.h>

int const Number_Count = 1000*1000*1000;
int const Batch_Size = 100;

/*
 * Generates random floating point numbers and writes them two a pipe. The GNU 
 * pipe function is used to facilitate this task. The constant Number_Count 
 * defines the number of random numbers written.
 *
 * @param argument Pointer to an integer representing the file descriptor of the
 *                 pipe to write to.
 *
 * @return Will always return NULL.
 */
void *producer(void *argument) 
{
    int a =  632559378;  /* \ Wichmann-Hill (2006) */
    int b = 1436901506;  /* | 4-cycle random */
    int c =  244914893;  /* | generator */
    int d = 1907460368;  /* / state. */

    double const recip_a /* 1/2 147 483 579.0 */ =
    0.00000000046566130226972971885062316464865503867958;
    double const recip_b /* 1/2 147 483 543.0 */ =
    0.00000000046566131007598599324865699331694482745566;
    double const recip_c /* 1/2 147 483 423.0 */ =
    0.00000000046566133609684213147940094716158374741503;
    double const recip_d /* 1/2 147 483 123.0 */ =
    0.00000000046566140114899519981000567798175892812360;

    double w[Batch_Size]; /* The random number. */

    int i, j; /* Loop counter. */

    int fd = *((int *) argument); /* File descriptor of pipe to write to. */

    ssize_t rc; 
    size_t bytes_written, bytes_to_write;

    for (i = 0; i < Number_Count;) {
        /* Fill the buffer, w, with new random numbers. */
        for (j = 0; j < Batch_Size; i++, j++) {
            w[j]  = recip_a * (double)(a = (int)((a * 11600LL) % 2147483579));
            w[j] += recip_b * (double)(b = (int)((b * 47003LL) % 2147483543));
            w[j] += recip_c * (double)(c = (int)((c * 23000LL) % 2147483423));
            w[j] += recip_d * (double)(d = (int)((d * 33000LL) % 2147483123));
            if (w[j] >= 2.0) w[j] -= 2.0;
            if (w[j] >= 1.0) w[j] -= 1.0;
        }
        /* Using GNU's pipe function to send the number to the other thread. */
        bytes_written = 0;
        bytes_to_write = sizeof w;
        do {
            rc = write(fd, &w[bytes_written/sizeof(double)], bytes_to_write);
            if (rc < 0) {
                printf("Failed to write to pipe: %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            bytes_written += (size_t)rc;
            bytes_to_write -= (size_t)rc;
        } while (0 < bytes_to_write);
    }

    return NULL;
}

/*
 * Reads floating point numbers from a pipe and sumurise them. The GNU pipe 
 * function is used to facilitate this task. The constant Number_Count defines 
 * the number of random numbers read. The summary consists of the mean and 
 * standard diviation of the numbers recieved. A simplified version of Spicer's 
 * provisional means algorithm is used when calulating the mean and standard
 * diviation. 
 *
 * @param argument Pointer to an integer representing the file descriptor of the
 *                 pipe to read from.
 *
 * @return Will always return NULL.
 */
void *consumer(void *argument) 
{
    int i, j;
    double x[Batch_Size], v;
    double mean = 0.0, sum2 = 0.0;

    int fd = *((int *) argument);

    ssize_t rc; 
    size_t bytes_read, bytes_to_read;

    for (i = 0; i < Number_Count;) {
        /* Fill the buffer, x, with new random numbers from the pipe. */
        bytes_read = 0;
        bytes_to_read = sizeof x;
        do {
            rc = read(fd, &x[bytes_read/sizeof(double)], bytes_to_read);
            if (rc < 0) {
                printf("Failed to read from pipe: %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            bytes_read += (size_t)rc;
            bytes_to_read -= (size_t)rc;
        } while (0 < bytes_to_read);
        /* Add this batch to summary. */
        for (j = 0; j < Batch_Size; i++, j++) {
            v = (x[j] - mean)/(i+1);
            sum2 += ((i+1)*v)*(i*v);
            mean += v;
        }
    }
    printf("Mean = %g, standard deviation = %g\n", mean, sqrt(sum2/(i-1)));

    return NULL;
}

/*
 * Creates a producer and consumer thread. Each is given a file descriptor for 
 * their appropriate end of the pipe. 
 *
 * @return Returns 0 on sussess, and 1 on failure.
 */
int main (void)
{
    int rc;
    pthread_t p, c;
    int mypipe[2];
     
    /* Create the pipe. */
    if (pipe (mypipe)) {
       printf("Pipe failed.\n");
       return EXIT_FAILURE;
    }

    rc = pthread_create(&c, NULL, consumer, &(mypipe[0]));
    if (rc != 0) {
        printf("Failed to create comsumer.\n");
        exit(EXIT_FAILURE);
    }
    rc = pthread_create(&p, NULL, producer, &(mypipe[1]));
    if (rc != 0) {
        printf("Failed to create producer.\n");
        exit(EXIT_FAILURE);
    }
    rc = pthread_join(p, NULL);
    if (rc != 0) {
        printf("Failed to join producer.\n");
        exit(EXIT_FAILURE);
    }
    rc = pthread_join(c, NULL);
    if (rc != 0) {
        printf("Failed to join consumer.\n");
        exit(EXIT_FAILURE);
    }

    close(mypipe[0]);
    close(mypipe[1]);

    return EXIT_SUCCESS;
}
