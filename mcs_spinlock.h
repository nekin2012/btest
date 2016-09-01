/* a simple mcs lock in user space, support only one lock */

#ifndef __MCS_SPINLOCK_H__
#define __MCS_SPINLOCK_H__

struct mcs_spinlock {                                                                                                                                                                         
	struct mcs_spinlock *next;
	volatile int locked; /* 1 if lock acquired */
}; 

struct mcs_baselock {
	struct mcs_spinlock *pbase;
	pthread_spinlock_t plock;
};

extern void mcs_spin_init(struct mcs_baselock *lock);
extern void mcs_spin_lock(struct mcs_baselock *lock, struct mcs_spinlock *node);
extern void mcs_spin_unlock(struct mcs_baselock *lock, struct mcs_spinlock *node);

#endif
