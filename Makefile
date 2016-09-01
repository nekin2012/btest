ALL=cs amdahl ci

all: $(ALL)

cs: cs_test.o task.o misc.o queue.o cal.o
	$(CC) $^ -g -lpthread -o $@

amdahl: amdahl.o task.o misc.o cal.o mcs_spinlock.o
	$(CC) $^ -g -lpthread -o $@

ci: ci.o misc.o cal.o
	$(CC) $^ -g -o $@

cs_test.o: cs_test.c misc.h task.h queue.h cal.h
task.o: task.c task.h misc.h
misc.o: misc.c misc.h
cal.o: cal.c
amdahl.o: amdahl.c misc.h task.h mcs_spinlock.h
ci.o: ci.c misc.h cal.h
mcs_spinlock.o: mcs_spinlock.c mcs_spinlock.h

%.o: %.c
	$(CC) -c -g -Wall -D_GNU_SOURCE $< -o $@

.PHONY: clean cleanall tags

clean:
	rm -f *.o $(ALL)

cleanall:
	rm -f *.o $(ALL) tags cscope.out core
	sudo rm -f perf perf.data

tags:
	ctags -R --extra=f
	cscope -Rb
