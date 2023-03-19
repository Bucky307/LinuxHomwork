# Makefile
# Connor Thorpen
# COSC 3750 Spring 2023
# Homework 6
# Mar 6, 2023
#
# This Makefile will be used for compiling the wyls program.
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f #Used for removing files

#Deletes object files and executable
.PHONY: clean

#Creates the executable
wytar: wytar.c myFunctions.c
	${CC} ${CFLAGS} wytar.c myFunctions.c -o wytar

#removes executable
clean:
	${RM} wytar a.out core.*
