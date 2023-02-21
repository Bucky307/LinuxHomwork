# Makefile
# Author: Buck Harris
# Date : Feb 17, 2023
#
# COSC 3750, Homework 4
#
# This is the makefile that will create our
# wycat executable depending on wycat.c. It also
# has clena and tidy if the user wants to use them.
CC=@gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f

.PHONY: clean tidy
#creates the wycat executable
wycat: wycat.c
	${CC} ${CFLAGS} -o wycat wycat.c
#removes core dump files
tidy:
	${RM} a.out core.*
#removes wycat
clean:
	${RM} wycat
