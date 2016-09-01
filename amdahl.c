#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>

#include "misc.h"
#include "task.h"
#include "cal.h"
#include "mcs_spinlock.h"

int cfg_p = 99;
int cfg_s = 1;
int cfg_q = 4;
int cfg_lock_type = 0;
void (*spin_lock_m)(void*);
void (*spin_unlock_m)(void*);

static long time_begin=0;

static pthread_spinlock_t lock;
static struct mcs_baselock mcslock;

struct sstat {
	long count;
	long latencyp; /* latency on paralled task */
	long latencys; /* latency on serial task */
}*stat;
#define STAT_LATENCY(i) (stat[i].latencyp+stat[i].latencys)
#define STAT_LATENCY_AVE(i) (STAT_LATENCY(i)/stat[i].count)
#define STAT_LATENCY_AVE_P(i) (stat[i].latencyp/stat[i].count)
#define STAT_LATENCY_AVE_S(i) (stat[i].latencys/stat[i].count)

void pthread_spin_lock_m(void *l) {
	pthread_spin_lock(&lock);
}

void pthread_spin_unlock_m(void *l) {
	pthread_spin_unlock(&lock);
}

void mcs_spin_lock_m(void *l) {
	struct mcs_spinlock *ll = (struct mcs_spinlock *)l;
	mcs_spin_lock(&mcslock, ll);
}

void mcs_spin_unlock_m(void *l) {
	struct mcs_spinlock *ll = (struct mcs_spinlock *)l;
	mcs_spin_unlock(&mcslock, ll);
}

void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "p:s:q:l:"MISC_OPT)) != -1) {
		switch(opt) {
			PARSE_MISC_OPT;
			PARSE_ARG_I('p', cfg_p);
			PARSE_ARG_I('s', cfg_s);
			PARSE_ARG_I('q', cfg_q);
			PARSE_ARG_I('l', cfg_lock_type);
			default:
				fprintf(stderr, "usage: %s [-p p] [-s s] [-q q]",
						argv[0]);
				exit(0);

		}
	}
}

void * thread_routin(void * arg) {
	struct task * tsk = arg;
	int i = (int)(intptr_t)tsk->arg;
	long time1, time2;
	struct mcs_spinlock ll;

	while(1) {
		time1 = get_timestamp();
		heavy_cal(10, cfg_p);
		time2 = get_timestamp();
		stat[i].latencyp += time2-time1;

		spin_lock_m(&ll);
		heavy_cal(20, cfg_s);
		spin_unlock_m(&ll);

		stat[i].latencys += get_timestamp() - time2;
		stat[i].count++;
	}
}

void int_handler(int signum) {
	int i;
	long sum=0;
	long lsum=0;
	long all_time = (get_timestamp() - time_begin)/1000;

	verbose("exit\nstat:\n");
	for(i=0; i<cfg_q; i++) {
		DIE_IF(!stat[i].count, "no count");
		verbose("stat[%d]=%ld(%ld,%ld)\n", i, 
			STAT_LATENCY_AVE(i),
			STAT_LATENCY_AVE_P(i),
			STAT_LATENCY_AVE_S(i));
		sum+=stat[i].count;
		lsum+=STAT_LATENCY(i);
	}
	if(cfg_brief) {
		printf("%ld, %ld\n", sum/all_time, lsum/sum);
	}else {
		printf("sum=%ld, ave=%ldk/s, latency=%ldms\n", sum, sum/all_time, lsum/sum);
	}

	pthread_spin_destroy(&lock);

	exit(signum);
}

int main(int argc, char *argv[]) {
	struct task **tasks;
	int i;
	int ret;

	cfg_ncpu = get_ncpu();

	parse_opt(argc, argv);

	if(cfg_lock_type==1) {
		spin_lock_m = mcs_spin_lock_m;
		spin_unlock_m = mcs_spin_unlock_m;
		mcs_spin_init(&mcslock);
	}else {
		spin_lock_m = pthread_spin_lock_m;
		spin_unlock_m = pthread_spin_unlock_m;
	}

	verbose("amdahl(p=%d, s=%d, q=%d)\n", cfg_p, cfg_s, cfg_q);

	DIE_IF(cfg_q>cfg_ncpu, "q(%d)>cpus(%d)\n", cfg_q, cfg_ncpu);

	tasks = (struct task **)malloc(cfg_q * sizeof(struct task *));
	DIE_IF(!tasks, "malloc");

	ret = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	DIE_IF(ret, "spin_init");

	stat = (struct sstat *)calloc(cfg_q, sizeof(struct sstat));
	DIE_IF(!stat, "calloc");

	signal(SIGINT, int_handler);

	time_begin = get_timestamp();
	for(i=0; i<cfg_q; i++)
		tasks[i] = create_task_on_cpu(thread_routin, (void *)(intptr_t)i, i);
	for(i=0; i<cfg_q; i++)
		join_task(tasks[i]);

	free(stat);

	return EXIT_SUCCESS;
}
