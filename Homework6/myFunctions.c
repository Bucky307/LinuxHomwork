#include "tar_header.h"
#include "myFunctions.h"
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<tar.h>
#include<grp.h>
#include<pwd.h>

//Function
void archiveDir(char* archiveName, char* objName, struct stat st){
  struct tar_header header;
  memset(&header, 0, 512);

  sprintf(header.name,"%s/", objName);
  sprintf(header.mode, "%07o", st.st_mode & 07777);
  sprintf(header.uid, "%07o", st.st_uid);
  sprintf(header.gid, "%07o", st.st_gid);
  memset(header.size, '0', 11);
  sprintf(header.mtime, "%011lo", (unsigned long)st.st_mtime);
  header.typeflag = DIRTYPE;
  strcpy(header.magic, TMAGIC);
  strcpy(header.version, TVERSION);

  struct passwd *pwd = getpwuid(st.st_uid);
  struct group *grp = getgrgid(st.st_gid);
  if(pwd != NULL){
    strcpy(header.uname, pwd->pw_name);
  }
  else{
    sprintf(header.uname, "%07o", st.st_uid);
  }
  if(grp != NULL) {
    strcpy(header.gname, grp->gr_name);
  }
  else{
    sprintf(header.gname, "%07o", st.st_gid);
  }

  memset(header.devmajor, '0', 8);
  memset(header.devminor, '0', 8);

  memset(header.chksum, ' ', 8);
  unsigned int sum = 0;
  const unsigned char * p = (const unsigned char *) &header;
  for(int i = 0; i < 512; i++) {
    sum += p[i];
  }
  sprintf(header.chksum, "%06o", (int)sum);

  FILE* archive = fopen(archiveName, "a");//open file
  if(archive == NULL) {//check error opening file
    fprintf(stderr, "Error: Unable to open archive\n");
  }
  else{
    size_t writeReturn;

    writeReturn= fwrite(&header, 1, 512, archive);
    if(writeReturn != 512) {//check errors writing
      fprintf(stderr, "Error writing to archive.\n");
    }
    fclose(archive);
  }

  struct stat fileStat;
  struct dirent* entry;
  char path[4096];
  DIR *dir = opendir(objName);//open directory
  if(dir == NULL){
    perror("Error opening directory.");
  }
  else{
    //while there are entries in the directory, add them to archive
    while((entry = readdir(dir)) != NULL){
      if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
        snprintf(path, 4096, "%s/%s", objName, entry->d_name);
        if(lstat(path, &fileStat) == -1){
          fprintf(stderr, "Error: unable to stat %s\n", entry->d_name);
        }
        else if(S_ISREG(fileStat.st_mode) || S_ISLNK(fileStat.st_mode)){
          archiveFile(archiveName, path, fileStat);
        }
        else if(S_ISDIR(fileStat.st_mode)){
          archiveDir(archiveName, path, fileStat);
        }
        else{
          fprintf(stderr, "Error: filetype not supported for %s\n", path);
        }
      }
    }
    closedir(dir);
  }
}

//Function
void archiveFile(char* archiveName, char* objName, struct stat st){
  struct tar_header header;
  memset(&header, 0, 512);

  strncpy(header.name, objName, 100);
  snprintf(header.mode, 8, "%07o", st.st_mode & 07777);
  snprintf(header.uid, 8, "%07o", st.st_uid);
  snprintf(header.gid, 8, "%07o", st.st_gid);
  snprintf(header.mtime, 12, "%011lo", (unsigned long)st.st_mtime);

  if(S_ISREG(st.st_mode)){
    header.typeflag = REGTYPE;
    snprintf(header.size, 12, "%011lo", (unsigned long)st.st_size);
  }
  else if(S_ISLNK(st.st_mode)){
    header.typeflag = SYMTYPE;
    memset(header.size, '0', 11);
    if(readlink(objName, header.linkname, 100) == -1){
      fprintf(stderr, "Error reading link");
    }
  }
  strcpy(header.magic, TMAGIC);
  strcpy(header.version, TVERSION);

  struct passwd *pwd = getpwuid(st.st_uid);
  struct group *grp = getgrgid(st.st_gid);
  if(pwd != NULL){
    strcpy(header.uname, pwd->pw_name);
  }
  else{
    sprintf(header.uname, "%07o", st.st_uid);
  }
  if(grp != NULL) {
    strcpy(header.gname, grp->gr_name);
  }
  else{
    sprintf(header.gname, "%07o", st.st_gid);
  }

  memset(header.devmajor, '0', 8);
  memset(header.devminor, '0', 8);

  //checksum
  memset(header.chksum, ' ', 8);
  unsigned int sum = 0;
  const unsigned char * p = (const unsigned char *) &header;
  for(int i = 0; i < 512; i++) {
    sum += p[i];
  }
  snprintf(header.chksum, 8, "%06o", (int)sum);

  //
  FILE* archive = fopen(archiveName, "a");//open file
  if(archive == NULL) {//check error opening file
    fprintf(stderr, "Error: Unable to open archive\n");
  }
  else{
    size_t readReturn, writeReturn;

    writeReturn= fwrite(&header, 1, 512, archive);
    if(writeReturn != 512) {//check errors writing
      fprintf(stderr, "Error writing to archive.\n");
    }
    if(S_ISREG(st.st_mode)){
      FILE* fp = fopen(objName, "r");
      if(fp == NULL){
        fprintf(stderr, "No such file or directory: %s \n", objName);
      }
      else{
        char buffer[512];
        do{
          readReturn = fread(buffer, 1, 512, fp);//read from file
          if(readReturn < 512) {//check errors reading
            if(ferror(fp)) {
              fprintf(stderr, "Error reading from file: %s.\n", objName);
            }
          }
          writeReturn = fwrite(buffer, 1, readReturn, archive);
          if(writeReturn != readReturn) {//check errors writing
            fprintf(stderr, "Error writing to archive.\n");
          }
        } while(readReturn == 512);
        if(readReturn>0){
          memset(&buffer, 0, 512);
          writeReturn = fwrite(buffer, 1, 512 - readReturn, archive);
          if(writeReturn != 512 - readReturn){//check errors writing
            fprintf(stderr, "Error writing to archive.\n");
          }
        }
      }
      fclose(fp);
    }
    fclose(archive);
  }

}


//Function
void extract(char* archiveName, char* objName){
  struct stat st;
  char buf[512];
  struct tar_header* header_ptr;
  size_t readReturn, writeReturn;

  FILE* archive = fopen(archiveName, "r");//open file
  if(archive == NULL) {//check error opening file
    fprintf(stderr, "Error: Unable to open archive\n");
  }
  else{
    do {
      readReturn = fread(buf, 1, 512, archive);//read from file
      if(readReturn < 512) {//check errors reading
        if(ferror(archive)) {
          fprintf(stderr, "Error reading from archive.\n");
        }
      }
      header_ptr = (struct tar_header*) buf;
      if(header_ptr->name[0] == '\0'){
        break;
      }
      if(strncmp(header_ptr->name, objName, 100) == 0){//extract this obj
        if(header_ptr->typeflag == DIRTYPE){
          
        }
        else if(header_ptr->typeflag == SYMTYPE){
          
        }
        else{
          if(header_ptr->typeflag != REGTYPE){
            fprintf(stderr, "Warning: extracting unsupported filetype.\n");
          }
          FILE* newFile = fopen(header_ptr->name, "w");
          if(newFile == NULL){
            fprintf(stderr, "Error extracting from archive\n");
          }
          else{
            size_t rRet, wRet;
            char contents[512];
            int size;
            sscanf(header_ptr->size, "%o", &size);
            for(int i = 0; i < size/512 + 1; i ++){
              rRet = fread(contents, 1, 512, archive);
              if(rRet < 512) {//check errors reading
                if(ferror(archive)) {
                  fprintf(stderr, "Error reading from archive.\n");
                }
              }
              if(i == size/512){
                wRet = fwrite(contents, 1, size - i*512, newFile);
                if(wRet != size - i*512) {//check errors writing
                  fprintf(stderr, "Error writing to file.\n");
                }
              }
              else{
                wRet = fwrite(contents, 1, rRet, newFile);
                if(wRet != rRet) {//check errors writing
                  fprintf(stderr, "Error writing to file.\n");
                }
              }
            }
            fclose(newFile);
          }
        }
      }
      else{//skip to next header

      }
    }while(readReturn == 512);
    fclose(archive);
  }
}


//Function
void extractAll(char* archiveName){
  struct stat fileStat;
  
}
