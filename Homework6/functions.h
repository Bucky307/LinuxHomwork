/*
 * functions.h
 * Author: Buck Harris
 * Date: Mar 19, 2023
 *
 * COSC 3750, Homework 6
 *
 * This is the header file for the funtions
 * in the functions.c file
 */
#include <sys/stat.h>
#include <stdio.h>

void archiveDirectory(FILE* archive, char* objName, struct stat st);
void archiveFile(FILE* archive, char* objName, struct stat st);
void extract(char* archiveName);
void extractParent(char* parentPath);
