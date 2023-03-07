/*
 * wycat.c
 * Author: Buck Harris
 * Date: Feb 17, 2023
 *
 * COSC 3750, Homework 4
 *
 * This is a simple version of the ls utility. It is designed to
 * take any amount of directories and options of n and h and
 * display information simmilar to ls -l. My implemetation
 * consists of several functions as it helped me organize my code
 * better.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void list_directory(char* directory_path, int n_flag, int h_flag);
void format_print(char* fileName, struct stat filestat, int n_flag, int h_flag);
void permissions(struct stat filestat);
void option_n(struct stat filestat, int n_flag);
void option_h(struct stat filestat, int h_flag);
void date_time(char* fileName, struct stat filestat);

//------------------------------------------
// Main function to get elements with argc and argv.
// it also checks to see if the element is an option.
// If it is a correct option, then it will call the
// list directory function.
int main(int argc, char **argv)
{
 int n_flag = 0; // flag for "-n" option
 int h_flag = 0; // flag for "-h" option
 int pathArgFound = 0;
 for (int i = 1; i < argc; i++)
 {
  if (argv[i][0] == '-' && !pathArgFound) // check if current argument is an option
  {
   int j = 1;
   while (argv[i][j] != '\0') // loop through each character of the option
   {
    if (argv[i][j] == 'n') // check for "-n" option
    {
     n_flag = 1;
    }
    else if (argv[i][j] == 'h') // check for "-h" option
    {
     h_flag = 1;
    }
    else
   {
    fprintf(stderr, "Option %s not supported in wyls\n", argv[i]);
    return 0;
   }
    j++;
   }
  }
  else // current argument is a directory
  {
   pathArgFound = 1;
   struct stat Stat;
   if (lstat(argv[i], &Stat) == -1)
   {
    fprintf(stderr, "Error no such directory or file: %s\n", argv[i]);
   }
   else
   {
    if (S_ISDIR(Stat.st_mode))
     list_directory(argv[i], n_flag, h_flag);
    else
     format_print(argv[i], Stat, n_flag, h_flag);
   }
  }
 }
 if (pathArgFound != 1)
 {
  char cwd[4096];
  if (getcwd(cwd, 4096) == NULL)
  {
   perror("getcwd() error");
  }
  else
   list_directory(cwd, n_flag, h_flag);
 }

 return 0;
}

//------------------------------------------
// This function works through the directory
// that was given and sends each of its elements
// to the format print function.
void list_directory(char* directory_path, int n_flag, int h_flag)
{
 DIR *dir = opendir(directory_path);

 if (dir != NULL)
 {
  struct dirent *entry;
  struct stat filestat;
  char path[4096];
  while ((entry = readdir(dir)) != NULL)
  {
   snprintf(path, 4096, "%s/%s", directory_path, entry->d_name);
   if (lstat(path, &filestat) == -1)
   {
    perror("stat error");
    continue;
   }
   if (entry->d_name[0] != '.')
    format_print(entry->d_name, filestat, n_flag, h_flag);
  }

 }
 else
  perror("opendir error");
 closedir(dir);
}

//------------------------------------------
// This function calls the permissions, options_n,
// and option_h functions with the filename given to it.
void format_print(char* fileName, struct stat filestat, int n_flag, int h_flag)
{
 // prints the permissions
 permissions(filestat);
 // prints the user and group
 option_n(filestat, n_flag);
 // prints the size
 option_h(filestat, h_flag);
 // prints the date and file name
 date_time(fileName, filestat);
}

//------------------------------------------
// Print the permissions of the files
void permissions(struct stat filestat)
{
 printf("%c%c%c%c%c%c%c%c%c%c ",
        (S_ISDIR(filestat.st_mode) ? 'd' :
        S_ISLNK(filestat.st_mode) ? 'l' :'-'),
        filestat.st_mode & S_IRUSR ? 'r' : '-',
        filestat.st_mode & S_IWUSR ? 'w' : '-',
        filestat.st_mode & S_IXUSR ? 'x' : '-',
        filestat.st_mode & S_IRGRP ? 'r' : '-',
        filestat.st_mode & S_IWGRP ? 'w' : '-',
        filestat.st_mode & S_IXGRP ? 'x' : '-',
        filestat.st_mode & S_IROTH ? 'r' : '-',
        filestat.st_mode & S_IWOTH ? 'w' : '-',
        filestat.st_mode & S_IXOTH ? 'x' : '-');
}

//------------------------------------------
// Prints the groups and user or group ID and,
// user ID depending on if there was an n option
void option_n(struct stat filestat, int n_flag)
{
 if (n_flag == 0)
 {
 // Get get the user and print the name
 // else print the filestat
  struct passwd *pwd = getpwuid(filestat.st_uid);
  if (pwd != NULL)
  {
   fprintf(stdout, "%-8s ", pwd->pw_name);
  }
  else
  {
   fprintf(stdout, "%-8d ", filestat.st_gid);
  }
 // get the group and print the name
 // else print the filestat
  struct group *grp = getgrgid(filestat.st_gid);
  if (grp != NULL)
  {
   fprintf(stdout, "%-8s ", grp->gr_name);
  }
  else
  {
   fprintf(stdout, "%-8d ", filestat.st_gid);
  }
 }
 else
  fprintf(stdout, "%-8d %-8d ", filestat.st_uid, filestat.st_gid);

}

//------------------------------------------
// Prints the size of the file in bytes or the,
// size in K M, or G depending if there was an
// h option.
void option_h(struct stat filestat, int h_flag)
{
 if (h_flag == 0)
  printf("%6ld ", filestat.st_size);
 else
 {
  float size = filestat.st_size;
  char unit = ' ';

  if (size >= 1024 * 1024 * 1024)
  {  // gigabytes
   size /= 1024 * 1024 * 1024;
   unit = 'G';
  }
  else if (size >= 1024 * 1024)
  {  // megabytes
   size /= 1024 * 1024;
   unit = 'M';
  }
  else if (size >= 1024)
  {  // kilobytes
   size /= 1024;
   unit = 'K';
  }

  if ((int)size == size)
  {
   fprintf(stdout, "%4d%c ", (int)size, unit);
  }
  else
  {
   fprintf(stdout, "%4.1f%c ", size, unit);
  }
 }
}

//------------------------------------------
// Print the date and time with correct formatting
// depending on its age.
void date_time(char* fileName, struct stat filestat)
{
 char date_str[80];
 time_t mtime = filestat.st_mtime;
 time_t now = time(NULL);
 double diff_secs = difftime(now, mtime);

 if (diff_secs > 60*60*24*180)
 {
   strftime(date_str, 80, "%b %e  %Y", localtime(&mtime));
 }
 else
 {
  strftime(date_str, 80, "%b %e %H:%M", localtime(&mtime));
  if (diff_secs < 0)
  {
   date_str[7] = ' ';
  }
 }

 if (S_ISDIR(filestat.st_mode))
 {
  fprintf(stdout, "%s %s/\n", date_str, fileName);
 }
 else if (S_ISLNK(filestat.st_mode))
 {
  char link_target[1024];
  ssize_t len = readlink(fileName, link_target, 1024);
  if (len != -1)
  {
   link_target[len] = '\0';
   fprintf(stdout, "%s %s -> %s\n", date_str, fileName, link_target);
  }
 }
 else
  fprintf(stdout,"%s %s\n", date_str, fileName);
}
