CC = gcc
OBJS1 = date.o event.o event_manager.o member.o event_manager_tests.o
EXEC1 = event_manager
OBJS2 = priority_queue.o priority_queue_tests.o pqNode.o
EXEC2 = priority_queue
DEBUG_FLAG = -g
COMP_FLAG = -std=c99 -Wall -Werror -pedantic-errors -DNDEBUG $(DEBUG_FLAG)

all: $(EXEC1) $(EXEC2)

# event_manager executable

$(EXEC1) : $(OBJS1)
	$(CC) $(DEBUG_FLAGS) $(OBJS1) -o $@ -L. -lpriority_queue

event_manager.o : event_manager.c priority_queue.h event_manager.h date.h event.h member.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
date.o : date.c date.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
event.o : event.c event.h date.h priority_queue.h member.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
member.o : member.c member.h date.h priority_queue.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
event_manager_tests.o : tests/event_manager_tests.c event_manager.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) tests/$*.c

# priority_queue executable

$(EXEC2) : $(OBJS2)
	$(CC) $(DEBUG_FLAGS) $(OBJS2) -o $@

priority_queue.o : priority_queue.c priority_queue.h pqNode.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
pqNode.o : pqNode.c pqNode.h priority_queue.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
priority_queue_tests.o : tests/priority_queue_tests.c priority_queue.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) tests/$*.c

clean:
	rm -f $(OBJS1) $(OBJS2) $(EXEC1) $(EXEC2)
