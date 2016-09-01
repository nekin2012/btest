#ifndef __TASK_H__
#define __TASK_H__

#include <pthread.h>

extern int cfg_ncpu;

struct task {
	pthread_t thread;
	pthread_attr_t attr;
	void * arg;
};

extern struct task * create_task_on_cpu(void *(*routin)(void *), void * arg, int cpu);
extern void join_task(struct task * tsk);
extern int get_ncpu(void);

static inline struct task * create_task(void *(*routin)(void *), void *arg) {
	return create_task_on_cpu(routin, arg, -1);
}


#endif
