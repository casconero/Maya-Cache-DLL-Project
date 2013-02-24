#include "FileUtility.h"

void getFileInfo(const char *completeFilePath, char **filePath, char ** fileName, char **fileExtension)
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
	extensionBookMark = strrchr(completeFilePath,'.');

	// searching last "\" (for name file)
	fileBookMark = strrchr(completeFilePath,'\\');

	pTemp = completeFilePath;
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




int nextFileSequence(int sequenceIndex)
{
	int sequenceLength=1;
	while (sequenceIndex>1)
	{
		sequenceLength++;
		sequenceIndex=sequenceIndex/10;
	}

	return sequenceLength;
}





int getSamplingRate(char *inputFile)
{
	int samplingRate;
	FILE *sourceFile;
	char line[1024];
	char *p,*q;
	char *t;

	// initialization
	samplingRate=0;
	p=NULL;
	q=NULL;

	if((sourceFile=fopen(inputFile,"rb"))!=NULL)
	{

		t=fgets(line,sizeof(char)*1024,sourceFile);
		while(t!=NULL)
		{
			if((p=strstr(line,"cacheTimePerFrame"))!=NULL)
				break;
			t=fgets(line,sizeof(char)*1024,sourceFile);
		}
	}

	if(p!=NULL)
	{
		// operation to get the sampling rate value
		p=strchr(p,'"');
		q=strrchr(p,'"');

		q--;
		while (q!=p)
		{
			samplingRate=atoi(q);
			q--;
		}
	}
	fclose(sourceFile);

	return samplingRate;
}

void buildNameFile(const char *path, const char*name ,const char * subfix, const char* extensionFileName,char**completeFileName)
{
	int dim=0;
	if(subfix!=NULL)
		dim=strlen(subfix);

	*completeFileName=(char*)malloc(sizeof(char)*(strlen(path)+strlen(name)+dim+strlen(extensionFileName)+1));
	memset(*completeFileName,'\0',sizeof(char));
	strcat(*completeFileName,path);
	strcat(*completeFileName,name);
	if(subfix!=NULL)
		strcat(*completeFileName,subfix);
	strcat(*completeFileName,extensionFileName);
}

