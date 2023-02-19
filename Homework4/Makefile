#Makefile
#Author: Buck Harris
#Date : Feb 17, 2023
#
#COSC 3750, Homework 4
#
#This is the makefile that will do stuff

CC=gcc
CFLAGS=-Wall -ggdb
RM=/bin/rm -f

.PHONY: clean tidy

wycat: wycat.c
	${CC} ${CFLAGS} -o wycat wycat.c

tidy: 
	${RM} a.out core.*

clean:
	${RM} wycat
