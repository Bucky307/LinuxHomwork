/********************
 * mm.c
 * Kim Buckner
 * COSC 3750
 * Spring
 *
 * This is a simple matrix multiplication program.
 * It has two options: 1) is an i-j-k loop to multiply the matrices
 * and 2) is an i-k-j loop. The student should be able to discuss why the
 * output of the two version is so different.
 ********************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char** argv)
{
  int option;

  if(argc==2) {
    option=atoi(argv[1]);
  }
  else {
    printf("usage: mm option\n");
    return 0;
  }

  int i,j,k;
  char a[5][10], b[10][6], c[5][6];

  memset(a,1,50);
  memset(b,1,60);
  memset(c,0,30);


  switch(option) {
    case 1:
      // the 'normal' i-j-k loop
      for(i=0;i< 5; i++ ) {
        for(j=0;j<10;j++) {
          for(k=0;k<6;k++) {
            c[i][k]+= a[i][j] * b[j][k];
            printf("%d ",c[i][k]);
          }
          printf("\n");
        }
      }
      break;
    case 2:
      // the i-k-j loop
      for(i=0;i< 5; i++ ) {
        for(k=0;k<6;k++) {
          for(j=0;j<10;j++) {
            c[i][k]+= a[i][j] * b[j][k];
            printf("%d ",c[i][k]);
          }
          printf("\n");
        }
      }
      break;
    default:
      printf("mm: option '%d' not supported\n",option);
      return 0;
  }

  for(i=0;i<5;i++) {
    for(k=0;k<6;k++) {
      if(k==0) printf("\n");
      printf("%d ",c[i][k]);
    }
  }

  printf("\n\n");

  return 0;

}

