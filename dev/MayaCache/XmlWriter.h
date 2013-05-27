#ifndef XMLWRITER_H_INCLUDED
#define XMLWRITER_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <MayaCache.h>

BOOL *isXmlFinalized;

void printXml(Channel *channel, int cacheId);

void writeXmlChannel(Channel *channel, int cacheId);

void closeXmlFile(int cacheId);

#endif