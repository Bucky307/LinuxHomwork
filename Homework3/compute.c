/****************************
 * compute.c
 * Kim Buckner
 * COSC3750
 * Homework 03
 * Performs the actual computation of a truncated series approximation 
 * of the constant e.
 ***************************/

#include<compute.h>

double  compute(int maxN) {
  int  n=0;
  double  factorialn=1, result=1;

  while(n < maxN) {
    n++;
    factorialn = factorialn / n; 
    result += factorialn;
  }
  return result;
}

