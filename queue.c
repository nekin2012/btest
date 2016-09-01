#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "queue.h"
#include "misc.h"

int cfg_q_size = 1000;
int cfg_q_timeout = 100; /* ms */

static long time1=0, time2=0;

DEF_STAT_VAR(full_hit);
DEF_STAT_VAR(empty_hit);
DEF_STAT_VAR(enq);
DEF_STAT_VAR(deq);
DEF_STAT_MAX(qlen);

#define QUEUE_SIZE 1000
#define MOVE(var) var = (var+1)%QUEUE_SIZE

static int *queue;
static int head=0, tail=0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condr = PTHREAD_COND_INITIALIZER;
pthread_cond_t condw = PTHREAD_COND_INITIALIZER;

static void print_q_stat(void) {
	time2 = get_timestamp();

	PRINT_STAT(full_hit);
	PRINT_STAT(empty_hit);
	PRINT_STAT(enq);
	PRINT_STAT(deq);
	PRINT_STAT_MAX(qlen);

	printf("\t\thandle %ldK per second\n", stat_deq*1000L/(time2-time1));
}

void init_q(void) {
	queue = (int *)malloc(sizeof(int)*cfg_q_size);
	DIE_IF(!queue, "malloc");
}

void fini_q(void) {
	print_q_stat();
	free(queue);
}

void en_q(int data) {
	int ret;
	int qlen;
	struct timespec ts;

	if(!time1)
		time1 = get_timestamp();

	ret = pthread_mutex_lock(&lock);
	DIE_IF(ret, "lock");

	while((head+1)%QUEUE_SIZE == tail) {
		STAT(full_hit);

		ret = pthread_cond_signal(&condr);
		DIE_IF(ret, "cond_signal");

		get_timeout(&ts, cfg_q_timeout);
		pthread_cond_timedwait(&condw, &lock, &ts);
	}

	queue[head] = data;
	MOVE(head);
	STAT(enq);

	qlen=head-tail;
	if(qlen<0)
		qlen+=QUEUE_SIZE;
	STAT_MAX(qlen, qlen);

	ret = pthread_mutex_unlock(&lock);
	DIE_IF(ret, "unlock");
}

int de_q(void) {
	int ret, data;
	struct timespec ts;

	ret = pthread_mutex_lock(&lock);
	DIE_IF(ret, "lock");

	while(head == tail) {
		STAT(empty_hit);

		ret = pthread_cond_signal(&condw);
		DIE_IF(ret, "cond_signal");

		get_timeout(&ts, cfg_q_timeout);
		pthread_cond_timedwait(&condr, &lock, &ts);
	}

	data = queue[tail];
	MOVE(tail);
	STAT(deq);

	pthread_mutex_unlock(&lock);
	DIE_IF(ret, "unlock");

	return data;
}

