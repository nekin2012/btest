#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/time.h>
#include "misc.h"

int cfg_set_marker = 0;
int cfg_brief = 0;

void DIE_check_func(char * f, int line, int cond, const char *fmt, ...) {
        va_list args;

        va_start(args, fmt);
        if(unlikely(cond)) {
                printf("DIE at %s:%i: ", f, line);
                vprintf(fmt, args);
                printf("\n");
                abort();
        }
        va_end(args);
}

/* get timestamp in microsecond */
long get_timestamp(void) {
	struct timeval t;
	int ret;

	ret = gettimeofday(&t, NULL);
	DIE_IF(ret, "gettimeofday");

	return t.tv_sec*1000000+t.tv_usec;
}

/* get timeout abs time after timeout microsecond */
void get_timeout(struct timespec * ts, long timeout) {
	struct timeval t;
	int ret = gettimeofday(&t, NULL);

	DIE_IF(ret, "gettimeofday");
	ts->tv_sec = t.tv_sec;
	ts->tv_nsec = t.tv_usec*1000+timeout;
}

void marker(const char *fmt, ...) {
        va_list args;
	FILE *mf;

	if(!cfg_set_marker)
		return;

	mf = fopen("/sys/kernel/debug/tracing/trace_marker", "w");

        va_start(args, fmt);
	vfprintf(mf, fmt, args);
        va_end(args);

	fclose(mf);
}
