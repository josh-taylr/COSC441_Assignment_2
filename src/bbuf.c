/* If you don't want N=30, T=int, #define those in the command line. */

#include "bbuf.h"

#ifdef TEST
#include <stdio.h>
#define DBG(x) (void)(x)
#ifdef __LP64__
#define PTRFMT "%12lu"
#else
#define PTRFMT "%8lu"
#endif
#else
#define DBG(x) (void)(0)
#endif

void bounded_buffer_init(bounded_buffer *p)
{
    pthread_mutexattr_t m;
    pthread_condattr_t c;

    pthread_mutexattr_init(&m);
    pthread_mutexattr_settype(&m, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&p->mutex, &m);
    pthread_condattr_init(&c);
    pthread_condattr_setpshared(&c, PTHREAD_PROCESS_PRIVATE);
    pthread_cond_init(&p->non_full, &c);
    pthread_cond_init(&p->non_empty, &c);
    p->head = p->tail = p->size = 0;
    pthread_condattr_destroy(&c);
    pthread_mutexattr_destroy(&m);
}

void bounded_buffer_destroy(bounded_buffer *p)
{
    pthread_cond_destroy(&p->non_empty);
    pthread_cond_destroy(&p->non_full);
    pthread_mutex_destroy(&p->mutex);
}

void bounded_buffer_add_last(bounded_buffer *p, T x)
{
    pthread_mutex_lock(&p->mutex);
    while (p->size == N) {
        DBG(printf(PTRFMT " wait non_full\n", (size_t) pthread_self()));
        pthread_cond_wait(&p->non_full, &p->mutex);
        DBG(printf(PTRFMT " wake non_full\n", (size_t) pthread_self()));
    }
    p->item[p->tail++] = x;
    DBG(printf(PTRFMT " add %d\n", (size_t) pthread_self(), x));
    if (p->tail == N)
        p->tail = 0;
    if (0 == p->size++)
        pthread_cond_broadcast(&p->non_empty);
    pthread_mutex_unlock(&p->mutex);
}

T 
bounded_buffer_remove_first(bounded_buffer * p)
{
    T x;

    pthread_mutex_lock(&p->mutex);
    while (p->size == 0) {
        DBG(printf(PTRFMT " wait non_empty\n", (size_t) pthread_self()));
        pthread_cond_wait(&p->non_empty, &p->mutex);
        DBG(printf(PTRFMT " wake non_empty\n", (size_t) pthread_self()));
    }
    x = p->item[p->head++];
    DBG(printf(PTRFMT " remove %x\n", (size_t) pthread_self(), x));
    if (p->head == N)
      p->head = 0;
  if (N == p->size--)
      pthread_cond_broadcast(&p->non_full);
  pthread_mutex_unlock(&p->mutex);
  return x;
}

#ifdef TEST2

#define N_PRODUCERS 10
#define N_ITEMS_PER_PRODUCER 10
#define N_CONSUMERS 10
#define N_ITEMS_PER_CONSUMER ((N_PRODUCERS*N_ITEMS_PER_PRODUCER)/N_CONSUMERS)

static bounded_buffer b;

static void *producer(void *x)
{
    int i;

    for (i = 0; i < N_ITEMS_PER_PRODUCER; i++) {
        bounded_buffer_add_last(&b, i);
    }
    return 0;
}

static void *consumer(void *x)
{
    int i;

    for (i = 0; i < N_ITEMS_PER_CONSUMER; i++) {
        int v = bounded_buffer_remove_first(&b);
    }
    return 0;
}

int main(void)
{
    pthread_t producer_id[N_PRODUCERS];
    pthread_t consumer_id[N_CONSUMERS];
    int i;

    bounded_buffer_init(&b);
    DBG(printf("starting\n"));
    for (i = 0; i < N_CONSUMERS; i++) {
        pthread_create(&consumer_id[i], 0, consumer, 0);
    }
    DBG(printf("consumers created\n"));
    for (i = 0; i < N_PRODUCERS; i++) {
        pthread_create(&producer_id[i], 0, producer, 0);
    }
    DBG(printf("producers created\n"));
    for (i = 0; i < N_CONSUMERS; i++) {
        pthread_join(consumer_id[i], 0);
    }
    for (i = 0; i < N_PRODUCERS; i++) {
        pthread_join(producer_id[i], 0);
    }
    DBG(printf("finished\n"));
    bounded_buffer_destroy(&b);
    return 0;
}

#endif
