#ifndef _CACHETRANSCODING_H_INCLUDED
#define _CACHETRANSCODING_H_INCLUDED

#include "Boolean.h"
#include "MayaNCache.h"
#include "FileUtility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define ONEFILEDECLARATION "OneFile"
#define ONEFILEPERFRAMEDECLARATION "OneFilePerFrame"
#define FRAMESUBFIX "Frame" 
#define ONEFILELENGTH 7
#define ONEFILEPERFRAMELENGTH 15
#define FRAMESUBFIXLENGTH 5

BOOL _MultiFileToSingleFileConverter(char *sourceFile, char * destinationFile);
BOOL _SingleFileToMultiFileConverter(char *sourceFile, char * destinationFile);

#endif