# Makefile
# Buck Harris
# COSC 3750 Spring 2023
# Homework 6
# Mar 19, 2023
#
# This Makefile will be used for compiling the wytar program.
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f #Used for removing files

#Deletes object files and executable
.PHONY: clean

#Creates the executable
wytar: wytar.c functions.c
	${CC} ${CFLAGS} wytar.c functions.c -o wytar

#removes executable
clean:
	${RM} wytar a.out core.*
