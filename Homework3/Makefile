##################################
# Makefile
# Buck Harris
# COSC 3750 Spring 2023
# Homework3
# Feb. 9, 2023
#
# This file is to implement a Makefile and have it do stuff
# to files tat will make an executable to compute e.
##################################
CC=gcc
CFLAGS=-Wall -ggdb
CPPFLAGS=-I.
RM=/bin/rm -f

OBJS=prompt.o compute.o display.o

.PHONY: clean tidy
#approxe is put first as it relies on the other targets
approxe: approxe.c $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o approxe approxe.c $(OBJS)
#below is where the tagets for the .c & .h files are created
prompt.o: prompt.c prompt.h
	${CC} ${CFLAGS} $(CPPFLAGS) -c prompt.c

compute.o: compute.c compute.h
	${CC} ${CFLAGS} $(CPPFLAGS) -c compute.c

display.o: display.c display.h
	${CC} ${CFLAGS} $(CPPFLAGS) -c display.c
#simple tidy and clean just like explained in notes
tidy:
	${RM} $(OBJS) a.out core.*

clean:
	${RM} approxe $(OBJS)
