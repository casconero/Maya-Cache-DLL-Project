#include "FileUtility.h"

void getFileInfo(char *completeFilePath, char **filePath, char ** fileName, char **fileExtension)
{
	char *extensionBookMark;
	char *fileBookMark;
	char *pTemp;
	char *pStart, *pStop;
	int dim;

	if (*filePath!=NULL)
	{
		free(*filePath);
		*filePath=NULL;
	}

	if (*fileName!=NULL)
	{
		free(*fileName);
		*fileName=NULL;
	}

	if (*fileExtension!=NULL)
	{
		free(*fileExtension);
		*fileExtension=NULL;
	}

	dim=1;

	//searching last "." (for extension)
	extensionBookMark=strrchr(completeFilePath,'.');

	// searching last "\" (for name file)
	fileBookMark=strrchr(completeFilePath,'\\');

	pTemp=completeFilePath;
	while(pTemp!=fileBookMark)
	{
		dim++;
		pTemp++;
	}

	*filePath=(char*)malloc(sizeof(char)*(dim));
	memmove(*filePath,completeFilePath,sizeof(char)*(dim+1));
	filePath[0][dim]='\0';

	pStop = extensionBookMark;
	pStart = fileBookMark;
	pStart++;
	dim=0;
	while(pStart!=pStop)
	{
		dim++;
		pStart++;
	}
	fileBookMark++;

	*fileName=(char*)malloc(sizeof(char)*dim);
	memmove(*fileName,fileBookMark,sizeof(char)*dim);
	fileName[0][dim]='\0';

	dim=1;
	pStart=extensionBookMark;
	while (*pStart!='\0')
	{
		dim++;
		pStart++;
	}

	*fileExtension=(char*)malloc(sizeof(char)*dim);
	memmove(*fileExtension,extensionBookMark,sizeof(char)*dim);
	fileExtension[0][dim]='\0';
}