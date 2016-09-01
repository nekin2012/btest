#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include "cal.h"
#include "misc.h"

int cfg_size=1000000; /* in pages */
int cfg_unit=100;    /* in pages */

static long time1=0, time2=0;

void int_handler(int signum) {
	time2 = get_timestamp();
	printf("exit\nstat:\n");
	printf("break in %ldus\n", time2-time1);
	exit(signum);

}

void parse_opt(int argc, char * argv[]) {
	int opt;

	while((opt=getopt(argc, argv, "s:u:")) != -1) {
		switch(opt) {
			PARSE_ARG_I('s', cfg_size);
			PARSE_ARG_I('u', cfg_unit);
			default:
				fprintf(stderr, "usage: %s [-s size] [-u unit]",
						argv[0]);
				exit(0);

		}
	}
}

int main(int argc, char *argv[]) {
	int i;

	parse_opt(argc, argv);
	DIE_IF(!cfg_unit, "cfg_unit");
	DIE_IF(!cfg_size, "cfg_unit");


	signal(SIGINT, int_handler);

	time1 = get_timestamp();
	for(i=0; i<cfg_size/cfg_unit; i++) {
		heavy_mem_write(cfg_unit);
	}
	time2 = get_timestamp();

	printf("finish in %ldus\n", time2-time1);

	return EXIT_SUCCESS;
}

