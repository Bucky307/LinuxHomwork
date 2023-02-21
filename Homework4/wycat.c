/*
 * wycat.c
 * Author: Buck Harris
 * Date: Feb 17, 2023
 *
 * COSC 3750, Homework 4
 *
 * This is a simple version of the cat utility. It is designed to
 * read from files or form the standardinput depending on what the 
 * user puts in before running.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void fileReadWrite(FILE *f)
{
char buffer[4096];
size_t ret, wret;
do
{
 ret = fread(buffer, 1, 4096, f);
 if (ret < 4096)
 {
  if (ferror(f))
  {
   fprintf(stderr, "Error reading from: %s\n", f);
  }
 wret = fwrite(buffer, 1, ret, stdout);
 if (wret != ret)
 {
  fprintf(stderr, "Error writing to: %s/n", f);
 }
}

}while (ret == 4096);
}




int main(int argc, char **argv){
 if (argc == 1)
 {
  fileReadWrite(stdin);
 }
 else
 {
  for(int i=1;i<argc;i++)
  {
   if(strcmp(argv[i],"-")==0)
   {
    fileReadWrite(stdin);
   }
   else
   {
FILE *f = fopen(argv[i],"r");
    if(!f)
    {
     fprintf(stderr,"%s: %s: No such file or directory\n", argv[0], argv[i]);
    }
    else
    {
     fileReadWrite(f);
     fclose(f);
    }
   }
  }
 }
 return 0;
}
