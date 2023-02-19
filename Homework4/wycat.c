/*
 * wycat.c
 * Author: Kim Buckner
 * Date: Feb 17, 2023
 *
 * COSC 3750, Homework 4
 *
 * This is a simple version of the cat utility. It is designed to
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
  char buffer[4096];
  size_t ret, wret;
  if (argc == 1){
    ret = fread(buffer, sizeof(*buffer), sizeof buffer, stdin);
    wret = fwrite(buffer, sizeof(*buffer), ret, stdout);
  }
  else{
    for(int i=1;i<argc;i++){
      if(strcmp(argv[i],"-")==0){
        ret = fread(buffer, sizeof(*buffer), sizeof buffer, stdin);
        wret = fwrite(buffer, sizeof(*buffer), ret, stdout);
      }
      else{
FILE *fp = fopen(argv[i],"r");
        if(!fp){
          fprintf(stderr,"%s: %s: No such file or directory\n", argv[0], argv[i]);
        }
else{
          ret = fread(buffer, sizeof(*buffer), sizeof buffer, fp);
          wret = fwrite(buffer, sizeof(*buffer), ret, stdout);
          fclose(fp);
        }
      }
    }
  }
  return 0;
}
