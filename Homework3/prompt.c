// prompt.c
// Kim Buckner
// COSC3750
// Homework 03
// 
// Function prompts for an integer to used to compute an approximation of 
// the constant e.

#include<prompt.h>

int  prompt(void) 
{
  int rtn;
  int n=-1; // value not in the possible range

  printf("Computing Approximation of e by Truncated Series\n");
  printf("================================================\n");

  while(n < 0) {
    printf("Enter max n for series sum (n>=0):  ");
    rtn=scanf("%d",&n);
    if(rtn == EOF)
    {
      perror("scanf:");
      return -1;
    }
  }
  return n;
}

