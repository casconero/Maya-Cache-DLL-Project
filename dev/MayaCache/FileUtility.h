#ifndef _FILEUTILITY_H_INCLUDED
#define _FILEUTILITY_H_INCLUDED

#include "Boolean.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void getFileInfo(const char *completeFilePath, char **filePath, char ** fileName, char **fileExtension);

int nextFileSequence(int sequenceIndex);

void buildNameFile(const char *path, const char*name ,const char * subfix, const char* extensionFileName,char**completeFileName);

int getSamplingRate(char *xmlSourceFile);

#endif