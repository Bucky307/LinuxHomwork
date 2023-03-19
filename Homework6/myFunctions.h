#include <sys/stat.h>

void archiveDir(char* archiveName, char* objName, struct stat st);
void archiveFile(char* archiveName, char* objName, struct stat st);
void extract(char* archiveName, char* objName);
void extractAll(char* archiveName);
