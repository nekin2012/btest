#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "misc.h"
#include "task.h"

int cfg_ncpu = 1;

int get_ncpu(void) {
	return sysconf(_SC_NPROCESSORS_ONLN);
}

struct task * create_task_on_cpu(void *(*routin)(void *), void *arg, int cpu) {
	struct task *tsk;
	cpu_set_t *cpuset;
	int cpuset_sz;
	int ret;


	tsk = (struct task *)malloc(sizeof(*tsk));
	DIE_IF(!tsk, "malloc");

	tsk->arg = arg;
	
	ret = pthread_attr_init(&tsk->attr);
	if(cpu>=0) {
		cpuset = CPU_ALLOC(cfg_ncpu);
		cpuset_sz = CPU_ALLOC_SIZE(cfg_ncpu);
		DIE_IF(!cpuset, "cpuset alloc");
		CPU_ZERO_S(cpuset_sz, cpuset);
		CPU_SET_S(cpu, cpuset_sz, cpuset);
		pthread_attr_setaffinity_np(&tsk->attr, cpuset_sz, cpuset);
		CPU_FREE(cpuset);
	}
	ret = pthread_create(&tsk->thread, &tsk->attr, routin, (void *)tsk);
	DIE_IF(ret, "pthread_create");


	return tsk;
}

void join_task(struct task * tsk) {
	pthread_join(tsk->thread, NULL);
	pthread_attr_destroy(&tsk->attr);
	free(tsk);
}
