/*
 * wycat.c
 * Author: Buck Harris
 * Date: Feb 17, 2023
 *
 * COSC 3750, Homework 4
 *
 * This is a simple version of the cat utility. It is designed to
 * read from files or form the standard input depending on what the
 * user puts in before running.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void fileReadWrite(FILE *file, char* fileName);

// This is the main function that checks for the
// differnet input types and sends them to the
// fileReadWrite function to to the "catting"
int main(int argc, char **argv)
{
 if (argc == 1)
 {
  fileReadWrite(stdin, "stdin");
 }
 else
 {
  for(int i=1;i<argc;i++)
  {
   if(strcmp(argv[i],"-") == 0)
   {
    fileReadWrite(stdin, "stdin");
   }
   else
   {
    FILE *file = fopen(argv[i],"r");
    if(!file)
    {
     fprintf(stderr,"%s: Error, no such file or directory: %s\n", argv[0], argv[i]);
    }
    else
    {
     fileReadWrite(file, argv[i]);
     fclose(file);
    }
   }
  }
 }
 return 0;
}

// The fileReadWrite function does the
// "catting" to whichever file is passes to it.
// It also makes sure that it can read from the file
// and write properly, and display an error if it
// finds one.
void fileReadWrite(FILE *file, char* fileName )
{
 char buffer[4096];
 size_t readReturn;
 size_t writeReturn;
 do
 {
  readReturn = fread(buffer, 1, 4096, file);
  if (readReturn < 4096)
  {
   if (ferror(file))
   {
    fprintf(stderr, "Error reading from: %s\n", fileName);
   }
   writeReturn = fwrite(buffer, 1, readReturn, stdout);
  }
  if (writeReturn != readReturn)
  {
   fprintf(stderr, "Error in writing to: %s\n ", fileName);
  }
 }while (readReturn == 4096);
}
