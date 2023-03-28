/*
 * wytar.c
 * Author: Buck Harris
 * Date: Mar 28, 2023
 *
 * COSC 3750, Homework 6
 *
 * This will either archive or extract file(s)
 * just like how the built in tar function does,
 * depending on what option the user uses.
 *
 */
#include "tar_header.h"
#include "functions.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<errno.h>
#include<tar.h>
#include<grp.h>
#include<pwd.h>
#include<utime.h>

//Function for Archiving Directories
void archiveDirectory(FILE* archive, char* objName, struct stat st)
{
 char name[101], prefix[156];
 size_t objNameLength = strlen(objName);

 if (objNameLength > 255)
 {
  fprintf(stderr, "Error object name %s is too large.\n", objName);
  return;
 }
 else if (objNameLength > 100)
 {
  char* last_slash = strrchr(objName, '/');
  if (last_slash != NULL && last_slash != objName+objNameLength-1)
  {
   // If the string contains a '/' and it is not the last character
   strncpy(prefix, objName, last_slash-objName);
   prefix[last_slash-objName] = '\0';
   strcpy(name, last_slash+1);
  }
  else
  {
   // If the string ends with a '/'
   strncpy(prefix, objName, objNameLength-1);
   prefix[objNameLength-1] = '\0';
   name[0] = '\0';
  }
 }
 else
 {
  strcpy(name, objName);
  prefix[0] = '\0';
 }
 // outputs all the data
 struct tar_header header;
 memset(&header, 0, 512);
 sprintf(header.name, "%s/", name);
 sprintf(header.mode, "%07o", st.st_mode & 07777);
 sprintf(header.uid, "%07o", st.st_uid);
 sprintf(header.gid, "%07o", st.st_gid);
 memset(header.size, '0', 11);
 sprintf(header.mtime, "%011lo", (unsigned long)st.st_mtime);
 header.typeflag = DIRTYPE;
 strcpy(header.magic, TMAGIC);
 strcpy(header.version, TVERSION);

 // If prefix is not empty, set it in the header.
 if (prefix[0] != '\0')
 {
  strcpy(header.prefix, prefix);
 }

 //for the group and user id
 struct passwd *pwd = getpwuid(st.st_uid);
 struct group *grp = getgrgid(st.st_gid);
 if(pwd != NULL)
 {
  strcpy(header.uname, pwd->pw_name);
 }
 else
 {
  sprintf(header.uname, "%07o", st.st_uid);
 }
 if(grp != NULL)
 {
  strcpy(header.gname, grp->gr_name);
 }
 else
 {
  sprintf(header.gname, "%07o", st.st_gid);
 }

 memset(header.devmajor, '0', 8);
 memset(header.devminor, '0', 8);

 memset(header.chksum, ' ', 8);
 unsigned int sum = 0;
 const unsigned char * p = (const unsigned char *) &header;
 for(int i = 0; i < 512; i++)
 {
  sum += p[i];
 }
 sprintf(header.chksum, "%06o", (int)sum);

 size_t writeReturn;
 writeReturn= fwrite(&header, 1, 512, archive);
 if(writeReturn != 512)
 {//check errors writing
  fprintf(stderr, "Error writing to archive.\n");
 }

 struct stat fileStat;
 struct dirent* entry;
 char path[4096];
 DIR *dir = opendir(objName);//open directory
 if(dir == NULL)
 {
  perror("Error opening directory.");
 }
 else
 {
  //while there are entries in the directory, add them to archive
  while((entry = readdir(dir)) != NULL)
  {
   if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0)
   {
    snprintf(path, 4096, "%s/%s", objName, entry->d_name);
    if(lstat(path, &fileStat) == -1)
    {
     fprintf(stderr, "Error: unable to stat %s\n", entry->d_name);
    }
    else if(S_ISREG(fileStat.st_mode) || S_ISLNK(fileStat.st_mode))
    {
     archiveFile(archive, path, fileStat);
    }
    else if(S_ISDIR(fileStat.st_mode))
    {
     archiveDirectory(archive, path, fileStat);
    }
    else
    {
     fprintf(stderr, "Error: filetype not supported for %s\n", path);
    }
   }
  }
  closedir(dir);
 }
}

//Function for Archiving Files
void archiveFile(FILE* archive, char* objName, struct stat st)
{

 char name[101], prefix[156];
 size_t objNameLength = strlen(objName);

 if (objNameLength > 255)
 {
  fprintf(stderr, "Error object name %s is too large.\n", objName);
  return;
 }
 else if (objNameLength > 100)
 {
  char* last_slash = strrchr(objName, '/');
  if (last_slash != NULL && last_slash != objName+objNameLength-1)
  {
   // If the string contains a '/' and it is not the last character
   strncpy(prefix, objName, last_slash-objName);
   prefix[last_slash-objName] = '\0';
   strcpy(name, last_slash+1);
  }
 }
 else
 {
  strcpy(name, objName);
  prefix[0] = '\0';
 }

 struct tar_header header;
 memset(&header, 0, 512);
 sprintf(header.name, "%s", name);
 sprintf(header.mode, "%07o", st.st_mode & 07777);
 sprintf(header.uid, "%07o", st.st_uid);
 sprintf(header.gid, "%07o", st.st_gid);
 memset(header.size, '0', 11);
 sprintf(header.mtime, "%011lo", (unsigned long)st.st_mtime);
 header.typeflag = DIRTYPE;
 strcpy(header.magic, TMAGIC);
 strcpy(header.version, TVERSION);

 // If prefix is not empty, set it in the header.
 if (prefix[0] != '\0')
 {
  strcpy(header.prefix, prefix);
 }
 if(S_ISREG(st.st_mode))
 {
  header.typeflag = REGTYPE;
  snprintf(header.size, 12, "%011lo", (unsigned long)st.st_size);
 }
 else if(S_ISLNK(st.st_mode))
 {
  header.typeflag = SYMTYPE;
  memset(header.size, '0', 11);
  if(readlink(objName, header.linkname, 100) == -1)
  {
   fprintf(stderr, "Error reading link");
  }
 }
 strcpy(header.magic, TMAGIC);
 strcpy(header.version, TVERSION);

 struct passwd *pwd = getpwuid(st.st_uid);
 struct group *grp = getgrgid(st.st_gid);
 if(pwd != NULL)
 {
  strcpy(header.uname, pwd->pw_name);
 }
 else
 {
  sprintf(header.uname, "%07o", st.st_uid);
 }
 if(grp != NULL)
 {
  strcpy(header.gname, grp->gr_name);
 }
 else
 {
  sprintf(header.gname, "%07o", st.st_gid);
 }

 memset(header.devmajor, '0', 8);
 memset(header.devminor, '0', 8);

 //checksum
 memset(header.chksum, ' ', 8);
 unsigned int sum = 0;
 const unsigned char * p = (const unsigned char *) &header;
 for(int i = 0; i < 512; i++)
 {
  sum += p[i];
 }
 snprintf(header.chksum, 8, "%06o", (int)sum);

 size_t readReturn, writeReturn;

 writeReturn= fwrite(&header, 1, 512, archive);
 if(writeReturn != 512)
 {//check errors writing
  fprintf(stderr, "Error writing to archive.\n");
 }
 if(S_ISREG(st.st_mode))
 {
  FILE* fp = fopen(objName, "r");
  if(fp == NULL)
  {
   fprintf(stderr, "No such file or directory: %s \n", objName);
  }
  else
  {
   char buffer[512];
   do
   {
    readReturn = fread(buffer, 1, 512, fp);//read from file
    if(readReturn < 512)
    {//check errors reading
     if(ferror(fp))
     {
      fprintf(stderr, "Error reading from file: %s.\n", objName);
     }
    }
    writeReturn = fwrite(buffer, 1, readReturn, archive);
    if(writeReturn != readReturn)
    {//check errors writing
     fprintf(stderr, "Error writing to archive.\n");
    }
   } while(readReturn == 512);
   if(readReturn>0)
   {
    memset(&buffer, 0, 512);
    writeReturn = fwrite(buffer, 1, 512 - readReturn, archive);
    if(writeReturn != 512 - readReturn)
    {//check errors writing
     fprintf(stderr, "Error writing to archive.\n");
    }
   }
  }
  fclose(fp);
 }
}

//Function
void extract(char* archiveName)
{
 char buf[512];
 struct tar_header* header_ptr;
 size_t readReturn; //writeRetrun
 char name[255];

 FILE* archive = fopen(archiveName, "r");//open file
 if(archive == NULL)
 {//check error opening file
  fprintf(stderr, "Error: Unable to open archive\n");
 }
 else
 {
  do
  {
   readReturn = fread(buf, 1, 512, archive);//read from file
   if(readReturn < 512)
   {//check errors reading
    if(ferror(archive))
    {
     fprintf(stderr, "Error reading from archive.\n");
    }
   }
   //get correct checksum

   //Check name and combine name & prefix
   header_ptr = (struct tar_header*) buf;
   if (header_ptr->name[0] == '\0')
    break;
   char checkSum[8];
   strcpy(checkSum, header_ptr->chksum);
   char calcCheckSum[8];
   memset(header_ptr->chksum, ' ', 8);
   unsigned int sum = 0;
   for (int i = 0; i < 512; i++)
    sum += buf[i];
   snprintf(calcCheckSum, 8, "%06o", (int)sum);
   if (strcmp(calcCheckSum, checkSum) != 0)
   {
    fprintf(stderr, "Error calculating chksum.\n");
    return;
   }
   if (header_ptr->prefix[0] != '\0')
    snprintf(name, 255, "%s/%s", header_ptr->prefix, header_ptr->name);
   else
    snprintf(name, 255, "%s", header_ptr->name);
   //Extract parent to get to root parent
   extractParent(name);

   int mode, uid, gid;
   sscanf(header_ptr->mode, "%o", &mode);
   sscanf(header_ptr->uid, "%o", &uid);
   sscanf(header_ptr->gid, "%o", &gid);
   if(header_ptr->typeflag == DIRTYPE)
   {
    struct stat st;
    if (stat(name, &st) == -1) // Check if directory does not exist
    {
     if (mkdir(name, mode) == -1)
     {
       fprintf(stderr, "Error extracting directory: %s\n",name);
     }
    }
    else
    {
     if (!S_ISDIR(st.st_mode))
     {
      fprintf(stderr, "Error: %s exists but is not a directory\n", name);
     }
     else
     {
      if(chmod(name, mode) == -1) // Set permissions
       fprintf(stderr, "Error setting permissions.\n");
     }
    }

    if(chown(name, uid, gid) == -1) // Set user and group
     fprintf(stderr, "Error extracting user id: %d and group id: %d.\n",
     uid, gid);

    struct utimbuf timebuf;
    time_t mtime;
    sscanf(header_ptr->mtime, "%lo", &mtime);
    timebuf.actime = mtime;
    timebuf.modtime = mtime;
    if (utime(name, &timebuf) == -1)
    {
     fprintf(stderr, "Error setting modification time for %s.\n", name);
    }
   }
   else if(header_ptr->typeflag == SYMTYPE)
   {
    if (symlink(header_ptr->linkname, name) == -1)
    {
     fprintf(stderr, "Error extracting link\n");
    }
   }
   else
   {
    if(header_ptr->typeflag != REGTYPE)
    {
     fprintf(stderr, "Warning: extracting unsupported filetype.\n");
    }
    FILE* newFile = fopen(name, "w");
    if(newFile == NULL)
    {
     fprintf(stderr, "Error opening file.\n");
    }
    else
    {
     size_t rRet, wRet;
     char contents[512];
     int size;
     sscanf(header_ptr->size, "%o", &size);
     for(int i = 0; i < (size+511)/512; i ++)
     {
      rRet = fread(contents, 1, 512, archive);
      if(rRet < 512)
      {//check errors reading
       if(ferror(archive))
       {
        fprintf(stderr, "Error reading from archive.\n");
       }
      }
      if(i == size/512)
      {
       wRet = fwrite(contents, 1, size - i*512, newFile);
       if(wRet != size - i*512)
       {//check errors writing
        fprintf(stderr, "Error writing to file.\n");
       }
      }
      else
      {
       wRet = fwrite(contents, 1, rRet, newFile);
       if(wRet != rRet)
       {//check errors writing
        fprintf(stderr, "Error writing to file.\n");
       }
      }
     }
     fclose(newFile);
     if(chmod(name, mode) == -1) // Set permissions
      fprintf(stderr, "Error setting permissions.\n");
     if(chown(name, uid, gid) == -1) // Set user and group
      fprintf(stderr, "Error extracting user id: %d and group id: %d.\n",
      uid, gid);
     struct utimbuf timebuf;
     time_t mtime;
     sscanf(header_ptr->mtime, "%lo", &mtime);
     timebuf.actime = mtime;
     timebuf.modtime = mtime;
     if (utime(name, &timebuf) == -1)
     {
      fprintf(stderr, "Error setting modification time for %s.\n", name);
     }
    }
   }
  }while(readReturn == 512);
  fclose(archive);
 }
}

//Function
void extractParent(char* parentPath)
{
 struct stat st;
 if (stat(parentPath, &st) != 0 || !S_ISDIR(st.st_mode))
 {
  char parentPathCopy[225];
  strcpy(parentPathCopy, parentPath);
  char* lastSlash = strrchr(parentPathCopy, '/');
  if (lastSlash != NULL && *(lastSlash + 1) != '\0')
  {
   *lastSlash = '\0';
   extractParent(parentPathCopy);

   // Check if the directory already exists
   if (stat(parentPathCopy, &st) == -1 || !S_ISDIR(st.st_mode))
   {
    int mode = 0777; // Default mode
    if (mkdir(parentPathCopy, mode) == -1)
    {
     fprintf(stderr, "Error creating directory: %s\n", parentPathCopy);
    }
   }
  }
 }
}
