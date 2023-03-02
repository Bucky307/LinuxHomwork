/*
 * wycat.c
 * Author: Buck Harris
 * Date: Feb 17, 2023
 *
 * COSC 3750, Homework 4
 *
 * This is a simple version of the cat utility. It is designed to
 * read from files or form the standard input depending on what the
 * user puts in before running.
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

void list_directory(char* directory_path);
void format_print(struct dirent* file, struct stat filestat);

int main(int argc, char **argv)
{
 char cwd[4096];
 if (getcwd(cwd, 4096) == NULL)
 {
  perror("getcwd() error");
  return 0;
 }
 else if (argc <2)
 {
  list_directory(cwd);
  return 0;
 }

 for (int i = 1; i < argc; i ++)
 {
  list_directory(argv[i]);
 }
 return 0;
}

void list_directory(char* directory_path)
{
 DIR *dir = opendir(directory_path);

 if (dir != NULL)
 {
  printf("Contents of directory %s:\n", directory_path);
  struct dirent *entry;
  struct stat filestat;
  char path[4096];
  while ((entry = readdir(dir)) != NULL)
  {
   snprintf(path, 4096, "%s/%s", directory_path, entry->d_name);
   if (stat(path, &filestat) == -1)
   {
    perror("stat error");
    continue;
   }
   if (entry->d_name[0] != '.')
    format_print(entry, filestat);
  }

 }
 else
  perror("opendir error");
 closedir(dir);
}

void format_print(struct dirent* file, struct stat filestat)
{
 printf("%c%c%c%c%c%c%c%c%c%c ",
        S_ISDIR(filestat.st_mode) ? 'd' : '-',
        filestat.st_mode & S_IRUSR ? 'r' : '-',
        filestat.st_mode & S_IWUSR ? 'w' : '-',
        filestat.st_mode & S_IXUSR ? 'x' : '-',
        filestat.st_mode & S_IRGRP ? 'r' : '-',
        filestat.st_mode & S_IWGRP ? 'w' : '-',
        filestat.st_mode & S_IXGRP ? 'x' : '-',
        filestat.st_mode & S_IROTH ? 'r' : '-',
        filestat.st_mode & S_IWOTH ? 'w' : '-',
        filestat.st_mode & S_IXOTH ? 'x' : '-');

 struct passwd *pwd = getpwuid(filestat.st_uid);
 if (pwd != NULL)
 {
  printf("%-8s ", pwd->pw_name);
 }
 else
 {
  printf("%-8d ", filestat.st_uid);
 }

 struct group *grp = getgrgid(filestat.st_gid);
 if (grp != NULL)
 {
  printf("%-8s ", grp->gr_name);
 }
 else
 {
  printf("%-8d ", filestat.st_gid);
 }

 printf("%-6ld ", filestat.st_size);

 char mode_str[80];
 time_t mtime = filestat.st_mtime;
 time_t now = time(NULL);
 double diff_secs = difftime(now, mtime);

if (diff_secs > 60*60*24*180)
 {
  strftime(mode_str, 80, "%b %e  %Y", localtime(&mtime));
 }
 else
 {
  strftime(mode_str, 80, "%b %e %H:%M", localtime(&mtime));
 }

 printf("%s %s\n", mode_str, file->d_name);
}
