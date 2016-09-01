#ifndef __QUEUE_H__
#define __QUEUE_H__

extern int cfg_q_size;
extern int cfg_q_timeout; /* ms */

extern void en_q(int data);
extern int de_q(void);
extern void init_q(void);
extern void fini_q(void);

#endif
