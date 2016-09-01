#include <pthread.h>
#include <assert.h>
#include "mcs_spinlock.h"

void mcs_spin_init(struct mcs_baselock *lock) {
	lock->pbase = NULL;
	pthread_spin_init(&lock->plock, PTHREAD_PROCESS_PRIVATE);
}

void mcs_spin_lock(struct mcs_baselock *lock, struct mcs_spinlock *node)
{
	struct mcs_spinlock *prev;

	node->locked = 0;
	node->next   = NULL;

	/* prev=xchg(&pbase, node) */
	pthread_spin_lock(&lock->plock);
	prev = lock->pbase;
	lock->pbase = node;
	if (prev == NULL) {
		pthread_spin_unlock(&lock->plock);
		return; /* now lock is taken */
	}

	/* point the last taker to this cpu */
	prev->next = node; /* point to this lock block*/
	pthread_spin_unlock(&lock->plock);

	/* Wait until the lock holder passes the lock down. */
	while(!node->locked);
}

void mcs_spin_unlock(struct mcs_baselock *lock, struct mcs_spinlock *node)
{
	struct mcs_spinlock *next;
       
	pthread_spin_lock(&lock->plock);

	next = node->next;

	if(next) {
		assert(!next->locked);
		next->locked = 1;
	}else {
		assert(lock->pbase==node);
		lock->pbase = NULL;
	}
	pthread_spin_unlock(&lock->plock);
}
