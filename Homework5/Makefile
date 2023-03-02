# Makefile
# Author: Buck Harris
# Date : Mar 1, 2023
#
# COSC 3750, Homework 5
#
# This is the makefile that will create our
# wycat executable depending on wyls.c. It also
# has clean and tidy if the user wants to use them.
CC=@gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f

.PHONY: clean tidy
#creates the wyls executable
wyls: wyls.c
	${CC} ${CFLAGS} -o wyls wyls.c
#removes core dump files
tidy:
	${RM} a.out core.*
#removes wycat
clean:
	${RM} wyls

