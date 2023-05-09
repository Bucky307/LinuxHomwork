/*
* make_mat.c
* Kim Buckner, 4 Dec, 2013
*
* $Author: kbuckner $
* $Date: 2016/12/01 21:28:52 $
*
* Simple program to generate files that can be used in a matrix pultiply
* program. File format is "m,n,data" where m is (binary integer) number of rows
* and n is (binary integer) number of columns and data the binary
* representations of the values in row-major storage. The commas are for
* documentation and are NOT in the file.
*
* Tried to test CI
*
* $Log: make_mat.c,v $
* Revision 1.6 2016/12/01 21:28:52 kbuckner
* Change use of ONES
*
* Revision 1.5 2016/12/01 21:22:04 kbuckner
* Tweaked the random numbers
*
* Revision 1.4 2016/12/01 21:11:58 kbuckner
* *** empty log message ***
*
* Revision 1.3 2015/06/02 14:57:08 kbuckner
* Last changes.
*
* Revision 1.2 2013/12/17 20:35:30 kbuckner
* using double
*
* Revision 1.1 2013/12/04 16:49:05 kbuckner
* Initial checkin
*
*
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#define DATATYPE double
int main(int argc, char **argv)
{
int ONES=0;
long int r_num;
int i,j,size,rtn;
int num_rows, num_cols;
FILE *fp;
DATATYPE* rowbuf;
DATATYPE newval;
if(argc !=4 && argc != 5) {
fprintf(stderr,"usage: make_mat filename num_rows num_cols [val]\n");
return 1;
}
if(argc==5) ONES=atoi(argv[4]);
num_rows=atoi(argv[2]);
num_cols=atoi(argv[3]);
if(num_rows <= 0 || num_cols <= 0) {
fprintf(stderr,"num_rows & num_cols must be > 0\n");
return 1;
}
fp=fopen(argv[1],"w+");
if(fp==NULL) {
perror(argv[1]);
return 1;
}
size=num_cols*sizeof(DATATYPE);
rowbuf=malloc(size);
if(rowbuf == NULL) {
perror("allocating rowbuf");
fclose(fp);
return 1;
}
fwrite(&num_rows,1,sizeof(int),fp);
fwrite(&num_cols,1,sizeof(int),fp);
if(ONES == 0) {
// I will generate random numbers so seed the generator
srandom(time(NULL));
}
for(i=0;i<num_rows;i++) {
for(j=0;j<num_cols;j++) {
if(ONES) {
newval=ONES;
}
else {
r_num=random();
r_num %= 100000;
newval=r_num/13842.931;
}
rowbuf[j]= newval;
//printf("%0.8f\n",newval);
}
rtn=fwrite(rowbuf,1,size,fp);
if(rtn != size) {
perror("writing to file");
fclose(fp);
return 1;
}
}
fclose(fp);
return 0;
}

