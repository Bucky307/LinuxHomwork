# Makefile
# Buck Harris
# COSC 3750 Spring 2023
# Homework 9
# April 1, 2023
#
# This Makefile will be used for compiling the wyshel program.
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f #Used for removing files

#Deletes object files and executable
.PHONY: clean

#Creates the executable
wyshell: wyshell.c wyscanner.c wyscanner.h wyshell
	${CC} ${CFLAGS} wyshell.c wyscanner.c -o wyshell

#removes executable
clean:
	${RM} wyshell a.out core.*
