/*
* wytalkC.c
* Author: Buck Harris
* Date: April 1, 2023
*
* COSC 3750, Homework 7
*
* This is the code for the client side
* of the wytalk program
*/
#include "socketfun.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
 int soc;
 char* hostname;
 if (argc < 2)
 {
  fprintf(stderr, "Error: must specify hostname.\n");
  exit(1);
 }
 else
 {
  hostname = argv[1];
 }

 if ((soc = request_connection(hostname, 51100)) < 0)
 {
  fprintf(stderr, "Error connecting to socket.\n");
  exit(1);
 }

 int bytesR, bytesW;
 while (1)
 {
//writing
  char outbuffer[1024];
  if (fgets(outbuffer, 1024, stdin) == NULL)
   break;
  outbuffer[strcspn(outbuffer, "\n")] = '\n';
  outbuffer[strcspn(outbuffer, "\n")+1] = '\0';
  if ((bytesW = write(soc, outbuffer, strlen(outbuffer))) <0)
  {
   fprintf(stderr, "Error writing to socket\n");
   break;
  }



  char inbuffer[1024];
  char* ptr = inbuffer;
  while((bytesR = read(soc, ptr, 1)) > 0)
  {// Reading
   if (*ptr =='\n')
    break;
   ptr++;
  }
  if (bytesR < 0) //check for error & close
  {
   perror("Error reading from socket");
   close(soc);
   exit(1);
  }
  else if (bytesR == 0)
  {
   exit(1);
  }
 // print message read
 *ptr = '\0';
 printf("%s\n", inbuffer);
 }
 close(soc);
 return 0;
}

