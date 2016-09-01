#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>

#include "misc.h"
#include "task.h"
#include "queue.h"
#include "cal.h"

/* global parameters for the test */
int cfg_n_pro = 1;
int cfg_n_con = 1;
int cfg_yield_method = 0;
int cfg_n_p_cal = 100; 
int cfg_n_c_cal = 100;

void (*yield_method_f)(void);

void ym0(void) {
}

void ym1(void) {
	pthread_yield();
}

void ym2(void) {
	sleep(0);
}

void * pro_routin(void * arg) {
	struct task * tsk = arg;
	int ret;

	srand((intptr_t)tsk->arg);

	while(1) {
		ret = heavy_cal(rand(), cfg_n_p_cal);
		en_q(ret);
		marker("yield here");
		yield_method_f();
	}
}

void * con_routin(void * arg) {
	int ret;

	ret = 0;
	while(1) {
		ret *= heavy_cal(de_q(), cfg_n_c_cal);
		yield_method_f();
	}
}

void int_handler(int signum) {
	printf("exit\nstat:");
	fini_q();
	exit(signum);
}

void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "p:c:P:C:y:q:Q:m")) != -1) {
		switch(opt) {
			PARSE_MISC_OPT;
			PARSE_ARG_I('p', cfg_n_pro);
			PARSE_ARG_I('c', cfg_n_con);
			PARSE_ARG_I('P', cfg_n_p_cal);
			PARSE_ARG_I('C', cfg_n_c_cal);
			PARSE_ARG_I('y', cfg_yield_method);
			PARSE_ARG_I('q', cfg_q_size);
			PARSE_ARG_I('Q', cfg_q_timeout);
			default:
				p_error("usage: %s [-p n_pro] "
						"[-c n_con] [-P n_p_cal] "
						"[-C n_c_cal] "
						"[-q queue_size] "
						"[-Q queue_timeout] "
						"[-y yield_method]",
						argv[0]);
				exit(0);

		}
	}

	verbose("cs: n_pro=%d, n_con=%d, n_p_cal=%d, n_c_cal=%d, q_size=%d, q_timeout=%d\n",
			cfg_n_pro, cfg_n_con, cfg_n_p_cal, cfg_n_c_cal, cfg_q_size, cfg_q_timeout);

}

int main(int argc, char * argv[]) {
	struct task **tasks;
	int i;

	parse_opt(argc, argv);

	init_q();

	switch(cfg_yield_method) {
		case 1:
			yield_method_f = ym1;
			break;
		case 2:
			yield_method_f = ym2;
			break;
		default:
			yield_method_f = ym0;
			break;
	}

	signal(SIGINT, int_handler);

	tasks = (struct task **)malloc((cfg_n_pro+cfg_n_con) * sizeof(struct task *));
	DIE_IF(!tasks, "malloc");

	for(i=0; i<cfg_n_pro; i++)
		tasks[i] = create_task(pro_routin, (void *)(intptr_t)rand());

	for(;i<cfg_n_pro+cfg_n_con;i++)
		tasks[i] = create_task(con_routin, NULL);

	for(i=0; i<cfg_n_pro+cfg_n_con; i++) 
		join_task(tasks[i]);

	return EXIT_SUCCESS;
}
