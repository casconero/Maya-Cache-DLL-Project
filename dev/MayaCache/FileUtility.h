#ifndef _FILEUTILITY_H_INCLUDED
#define _FILEUTILITY_H_INCLUDED

#include "Boolean.h"
#include <string.h>
#include <stdio.h>



void getFileInfo(char *completeFilePath, char **filePath, char ** fileName, char **fileExtension);

#endif