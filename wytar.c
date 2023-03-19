/*
* wytar.c
* Author: Connor Thorpen
* Date: Mar 13, 2023
*
* COSC 3750, Homework 5
*
* This is a simple version of the tar utility.
*/

#include "tar_header.h"
#include "myFunctions.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <tar.h>
#include <grp.h>
#include <pwd.h>

int main(int argc, char **argv){
  int cOption = 0;
  int fOption = 0;
  int xOption = 0;
  int foundArchiveArg = 0;
  int foundFileArg = 0;
  char* archiveName = NULL;

  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-' && !foundArchiveArg){//option arguments
      int j = 1;
      while(argv[i][j] != '\0'){
        if(argv[i][j] == 'c'){
          if(xOption == 1){
            fprintf(stderr, "Error: Cannot use option c and x at the same time.\n");
            return 0;
          }
          cOption = 1;
        }
        else if(argv[i][j] == 'x'){
          if(cOption == 1){
            fprintf(stderr, "Error: Cannot use option c and x at the same time.\n");
            return 0;
          }
          xOption = 1;
        }
        else if(argv[i][j] == 'f'){
          fOption = 1;
        }
        else{
          fprintf(stderr, "Option not supported: %c\n", argv[i][j]);
          return 0;
        }
      j++;
      }
    }
    else{//pathname arguments
      if(fOption == 0 || (cOption == 0 && xOption == 0)){//check option errors
        fprintf(stderr, "Error: Must use option f and one of option c or x.\n");
        return 0;
      }
      if(!foundArchiveArg){//set the archive name
        foundArchiveArg = 1;
        archiveName = argv[i];
      }
      else if(strcmp(argv[i], archiveName) != 0 && cOption){//add to archive
        foundFileArg = 1;
        struct stat st;
        if(lstat(argv[i], &st) == -1){
          fprintf(stderr, "Error: No such file or directory:  %s\n", argv[i]);
        }
        else{
          if(S_ISDIR(st.st_mode)){
            archiveDir(archiveName, argv[i], st);
          }
          else if(S_ISLNK(st.st_mode) || S_ISREG(st.st_mode)){
            archiveFile(archiveName, argv[i], st);
          }
          else{
            fprintf(stderr, "Error: filetype not supported");
          }
        }
      }
      else if(strcmp(argv[i], archiveName) != 0 && xOption){//extract from archive
        foundFileArg = 1;
        extract(archiveName, argv[i]);
      }
    }
  }

  if(!foundArchiveArg){//no arguments (after options)
    if(fOption == 0 || (cOption == 0 && xOption == 0)){//check option errors
      fprintf(stderr, "Error: Must use option f and one of option c or x.\n");
      return 0;
    }
    else{
      fprintf(stderr, "Error: Must specify archive name \n");
      return 0;
    }
  }

  if(cOption){//c option used and we have archive name
    if(foundFileArg){
      size_t writeReturn;
      char buf[1024];
      memset(&buf, 0, 1024);
      FILE* fp = fopen(archiveName, "a");
      if(fp == NULL){
        fprintf(stderr, "Error writing to archive.\n");
      }
      else{
        writeReturn = fwrite(buf, 1, 1024, fp);
        if(writeReturn != 1024){
          fprintf(stderr, "Error writing to archive.\n");
        }
      }
    }
    else{
      fprintf(stderr, "Error: Must specify files to be added to archive.\n");
      return 0;
    }
  }
  else{//x option used and we have archive name
    if(!foundFileArg){
      //EXTRACT ALL FILES
      extractAll(archiveName);
    }
  }

  return 0;
}
