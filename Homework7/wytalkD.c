/*
* wytalkD.c
* Author: Buck Harris
* Date: April 1, 2023
*
* COSC 3750, Homework 7
*
* This is the code for the server side
* of the wytalk program
*/
#include "socketfun.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
 int soc1, soc2;
 char hostname[256];

 if (gethostname(hostname, 256) < 0)
 {
  fprintf(stderr, "Error: retrieving hostname.\n");
  exit(1);
 }
 printf("Hostname: %s\n", hostname);

 if ((soc1 = serve_socket(hostname, 51100)) < 0)
 {
  fprintf(stderr, "Error: creating socket.\n");
  exit(1);
 }

 if ((soc2 = accept_connection(soc1)) < 0)
 {
  fprintf(stderr, "Error: accepting connection.\n");
  exit(1);
 }

 int bytesR, bytesW;
 while (1)
 {
  char inbuffer[1024];
  char* ptr = inbuffer;
  while((bytesR = read(soc2, ptr, 1)) > 0)
  {// Reading
   if (*ptr =='\n')
    break;
   ptr++;
  }
  if (bytesR < 0) //Print error & close
  {
   perror("Error: reading from socket");
   close(soc2);
   close(soc1);
   exit(1);
  }
  else if (bytesR == 0)
  {
   exit(1);
  }
  *ptr = '\0';
  printf("%s\n", inbuffer);
  //writing
  char outbuffer[1024];
  if (fgets(outbuffer, 1024, stdin) == NULL)
   break;
  outbuffer[strcspn(outbuffer, "\n")] = '\n';
  outbuffer[strcspn(outbuffer, "\n")+1] = '\0';
  if ((bytesW = write(soc2, outbuffer, strlen(outbuffer))) <0)
  {
   fprintf(stderr, "Error: writing to socket\n");
   break;
  }
 }
 //Close sockets
 close(soc1);
 close(soc2);
 return 0;
}
