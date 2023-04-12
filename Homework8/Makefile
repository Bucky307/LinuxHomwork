# Makefile
# Buck Harris
# COSC 3750 Spring 2023
# Homework 7
# April 1, 2023
#
# This Makefile will be used for compiling the wytalk program.
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f #Used for removing files

#Deletes object files and executable
.PHONY: clean

#Creates the executable
wytalkC: wytalkC.c socketfun.c socketfun.h wytalkD
	${CC} ${CFLAGS} wytalkC.c socketfun.c -o wytalkC

wytalkD: wytalkD.c socketfun.c socketfun.h
	${CC} ${CFLAGS} wytalkD.c socketfun.c -o wytalkD
#removes executable
clean:
	${RM} wytalkC wytalkD a.out core.*
