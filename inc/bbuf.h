#ifndef BBUF_H_
#define BBUF_H_ 2011

#include <pthread.h>

#ifndef N
#define N 30
#endif
#ifndef T
#define T int
#endif

typedef struct bounded_buffer {
    pthread_mutex_t mutex;
    pthread_cond_t  non_full;  /* used to wait for queue not to be full */
    pthread_cond_t  non_empty; /* used to wait for queue not to be empty */
    int             head;
    int             tail;
    int             size;
    T               item[N];
} bounded_buffer;

extern void bounded_buffer_init(bounded_buffer *);
extern void bounded_buffer_destroy(bounded_buffer *);
extern void bounded_buffer_add_last(bounded_buffer *, T);
extern T    bounded_buffer_remove_first(bounded_buffer *);

#endif

