#ifndef __MISC_H__
#define __MISC_H__
#include <stdio.h>

extern int cfg_set_marker;
extern int cfg_brief;

extern void DIE_check_func(char * f, int line, int cond, const char *fmt, ...);
extern long get_timestamp(void);
extern void get_timeout(struct timespec * ts, long timeout);
extern void marker(const char *fmt, ...);

#define DIE_IF(cond, fmt, ...) DIE_check_func(__FILE__, __LINE__, !!(cond), fmt, ##__VA_ARGS__)
#define DIE(fmt, ...) DIE_IF(0, fmt, ...)

#define verbose(fmt, ...) if(!cfg_brief) printf(fmt, ##__VA_ARGS__)
#define p_error(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

#define DEF_STAT_VAR(var) static volatile sig_atomic_t stat_##var = 0
#define STAT(var) stat_##var++
#define PRINT_STAT(var) printf("\tstat_" #var ":\t\t%d\n", stat_##var)

#define DEF_STAT_MAX(var) static volatile sig_atomic_t stat_max_##var = 0
#define STAT_MAX(var, v) if(v > stat_max_##var) stat_max_##var=v
#define PRINT_STAT_MAX(var) printf("\tstat_max_" #var ":\t\t%d\n", stat_max_##var)

#define PARSE_ARG_I(key, arg) case key: arg = atoi(optarg); break
#define PARSE_ARG(key, arg) case key: arg = 1; break

#define PARSE_MISC_OPT \
	PARSE_ARG('b', cfg_brief); \
	PARSE_ARG('m', cfg_set_marker)

#define MISC_OPT "bm"

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#endif
