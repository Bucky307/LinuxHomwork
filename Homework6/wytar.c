/*
* wytar.c
* Author: Buck Harris
* Date: Mar 28, 2023
*
* COSC 3750, Homework 6
*
* This is the main function for the
* tar utility. It handles the options and
* calls the correct funcion to do what the
* user asked for.
*/

#include "tar_header.h"
#include "functions.h"
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

int main(int argc, char **argv)
{
 int option_c = 0;
 int option_f = 0;
 int option_x = 0;
 int foundArchiveArg = 0;
 int foundFileArg = 0;
 char* archiveName = NULL;
 FILE* archive;

 for(int i = 1; i < argc; i++)
 {
  if(argv[i][0] == '-' && !foundArchiveArg)
  {//option arguments
   int j = 1;
   while(argv[i][j] != '\0')
   {
    if(argv[i][j] == 'c')
    {
     if(option_x == 1)
     {
      fprintf(stderr, "Error: Cannot use option c and x at the same time.\n");
      return 0;
     }
     option_c = 1;
    }
    else if(argv[i][j] == 'x')
    {
     if(option_c == 1)
     {
      fprintf(stderr, "Error: Cannot use option c and x at the same time.\n");
      return 0;
     }
     option_x = 1;
    }
    else if(argv[i][j] == 'f')
    {
     option_f = 1;
    }
    else
    {
     fprintf(stderr, "Option not supported: %c\n", argv[i][j]);
     return 0;
    }
    j++;
   }
  }
  else
  {//pathname arguments
   if(option_f == 0 || (option_c == 0 && option_x == 0))
   {//check option errors
    fprintf(stderr, "Error: Must use option f and one of option c or x.\n");
    return 0;
   }
   if(!foundArchiveArg)
   {//set the archive name
    foundArchiveArg = 1;
    archiveName = argv[i];
    if (option_x)
    {
     extract(archiveName);
     break;
    }
   }
   else if(strcmp(argv[i], archiveName) != 0 && option_c)
   {//add to archive
    if (!foundFileArg)
    {
     foundFileArg = 1;
     archive = fopen(archiveName, "w");
     if (archive == NULL)
     {
      fprintf(stderr, "Error: archive %s not found.\n", archiveName);
      return 0;
     }
    }
    struct stat st;
    if(lstat(argv[i], &st) == -1)
    {
     fprintf(stderr, "Error: No such file or directory:  %s\n", argv[i]);
    }
    else
    {
     if(S_ISDIR(st.st_mode))
     {
      archiveDirectory(archive, argv[i], st);
     }
     else if(S_ISLNK(st.st_mode) || S_ISREG(st.st_mode))
     {
      archiveFile(archive, argv[i], st);
     }
     else
     {
      fprintf(stderr, "Error: filetype not supported");
     }
    }
   }
  }
 }

 if(!foundArchiveArg)
 {//no arguments (after options)
  if(option_f == 0 || (option_c == 0 && option_x == 0))
  {//check option errors
   fprintf(stderr, "Error: Must use option f and one of option c or x.\n");
   return 0;
  }
  else
  {
   fprintf(stderr, "Error: Must specify archive name \n");
   return 0;
  }
 }

 if(option_c)
 {//c option used and we have archive name
  if(foundFileArg)
  {
   size_t writeReturn;
   char buf[1024];
   memset(&buf, 0, 1024);
   writeReturn = fwrite(buf, 1, 1024, archive);
   if(writeReturn != 1024)
   {
    fprintf(stderr, "Error writing to archive.\n");
   }
   fclose(archive);
  }
  else
  {
   fprintf(stderr, "Error: Must specify files to be added to archive.\n");
   return 0;
  }
 }
 return 0;
}
