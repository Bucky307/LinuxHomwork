/****************************
 * approxe.c
 * Kim Buckner
 * COSC3750
 * Homework 03
 * Compute a truncated series approximation of 
 * the constant e.
 ***************************/

#include<prompt.h>
#include<compute.h>
#include<display.h>


/****************
 * Nice and clean, easy to follow
 ***************/
int main()
{
  int  n;
  double e;

  n = prompt();
  e = compute(n);
  display(e);

  return 0;
}
