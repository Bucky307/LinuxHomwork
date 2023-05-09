# Makefile
# Buck Harris
# COSC 3750 Spring 2023
# Homework 11
# April 1, 2023
#
# This Makefile will be used for compiling the mmult.c program.
CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f #Used for removing files

#Deletes object files and executable
.PHONY: clean

# Default target to build all executables
all: mmult mm dump make_mat

#Creates the mmult executable
mmult: mmult.c
	${CC} ${CFLAGS} mmult.c -o mmult -lpthread
#Creates the mm executable
mm: mm.c
	${CC} mm.c -o mm
#Creates the dump executable
dump: dump.c
	${CC} ${CFLAGS} dump.c -o dump
#Creates the make_mat executable
make_mat: make_mat.c
	${CC} ${CFLAGS} make_mat.c -o make_mat

#removes executable
clean:
	${RM} mmult dump mm make_mat a.out core.*