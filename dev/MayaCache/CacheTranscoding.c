#include "CacheTranscoding.h"


BOOL xmlConverter(char *xmlSourceFile, char *xmlDestinationFile,CACHEFORMAT sourceFormat, CACHEFORMAT destinationFormat);

int nextFileSequence(int sequenceIndex);

void retrieveFileName(char * fileNameInput, char ** fileNameOutput, int *sourceFileLength);

void sequenceFile(char **sequence, int frame, int *length);

//void getFileName(char **sourceCacheFileName,const char *sourceFile);

void getSourceCacheFileName(char **sourceCacheFileName,const char sourceFile, const int  frame);

// utilities functions
BOOL xmlConverter(char *xmlSourceFile, char *xmlDestinationFile,CACHEFORMAT sourceFormat, CACHEFORMAT destinationFormat);

int getSamplingRate(char *xmlSourceFile);

int nextFileSequence(int sequenceIndex);

void retrieveFileName(char * fileNameInput, char ** fileNameOutput, int *sourceFileLength);

void sequenceFile(char **sequence, int frame, int *length);

void getSourceCacheFileName(char **sourceCacheFileName,const char sourceFile, const int  frame);

BOOL xmlConverter(char *xmlSourceFile, char *xmlDestinationFile, CACHEFORMAT sourceFormat, CACHEFORMAT destinationFormat)
{
	BOOL xmlFileConverted = TRUE;
	BOOL replaced = FALSE;
	FILE *sourceXmlFile;
	FILE *destinationXmlFile;
	char *p;
	char *index;
	char row[1024];
	char newRow[1024];
	int i=0;

	// Opening file in write mode
	if((destinationXmlFile=fopen(xmlDestinationFile,"w"))==NULL)
		return FALSE;

	// Opening file in read mode
	if((sourceXmlFile=fopen(xmlSourceFile,"r"))==NULL)
	{
		fclose(destinationXmlFile);
		return FALSE;
	}

	// copying file source--->destination
	while ((fgets(row,sizeof(row),sourceXmlFile)!=NULL))
	{
		// need only to replace "OneFile" or "OneFilePerFrame"
		// according with the selected options
		if(!replaced)
		{
			memset(newRow,' ',1024);
			index=row;
			if(sourceFormat!=destinationFormat)
			{		 
				switch(sourceFormat)
				{
				case ONEFILEPERFRAME:
					if((p=strstr(row,ONEFILEPERFRAMEDECLARATION))!=NULL)
					{
						while(index!=p)
						{
							newRow[i]=row[i];
							index++;
							i++;
						}
						newRow[i]='\0';
						strcat(newRow,ONEFILEDECLARATION);
						strcat(newRow,&row[i+ONEFILEPERFRAMELENGTH]);
						fputs(newRow,destinationXmlFile);

						replaced=TRUE;
					}
					break;
				case ONEFILE:
					if((p=strstr(row,ONEFILEDECLARATION))!=NULL)
					{
						while(index!=p)
						{
							newRow[i]=row[i];
							index++;
							i++;
						}
						newRow[i]='\0';
						strcat(newRow,ONEFILEPERFRAMEDECLARATION);
						strcat(newRow,&row[i+ONEFILELENGTH]);
						fputs(newRow,destinationXmlFile);

						replaced=TRUE;
					}
					break;
				default:
					break;
				}
			}
			if(!replaced)
				fputs(row,destinationXmlFile);
		}
		else
			fputs(row,destinationXmlFile);

		fflush(destinationXmlFile);
	}
	fclose(destinationXmlFile);
	fclose(sourceXmlFile);
	return xmlFileConverted;
};

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

void retrieveFileName(char * fileNameInput, char ** fileNameOutput, int *sourceFileLength)
{
	char *p, *index;				// char pointer
	int dim;

	if (*fileNameOutput!=NULL)
	{
		free(*fileNameOutput);
		*fileNameOutput=NULL;
	}

	if((p=strstr(fileNameInput,XMLEXTENSION))!=NULL)
	{
		//if extension found 
		dim=1;
		index=fileNameInput;
		while(index!=p)
		{
			index++;
			dim++;
			*sourceFileLength++;
		}
		// excluding "." 
		dim--;
	}

	sourceFileLength=&dim;
	*fileNameOutput=(char*)calloc(dim-1+FRAMESUBFIXLENGTH, sizeof(char));
	memmove(*fileNameOutput,fileNameInput,sizeof(char)*dim);
	fileNameOutput[0][dim]='\0';
}

void sequenceFile(char **sequence, int frame, int *length)
{
	int sequenceFileLength;
	char * sequenceChar;

	if (*sequence!=NULL)
	{
		free(*sequence);
		*sequence=NULL;
	}

	sequenceFileLength=nextFileSequence(frame);
	sequenceChar=(char *)malloc(sizeof(char)*(sequenceFileLength+1));
	itoa(frame,sequenceChar,10);
	sequenceChar[sequenceFileLength]='\0';
	*sequence=(char*)malloc(sizeof(char)*(sequenceFileLength+MAYACACHEEXTENSIONLENGTH+1));
	memmove(*sequence,sequenceChar,sizeof(char)*(sequenceFileLength+1));
	strcat(*sequence,MAYACACHEEXTENSION);
	*length=sequenceFileLength+MAYACACHEEXTENSIONLENGTH;
}

int getSamplingRate(char *inputFile)
{
	int samplingRate,i;
	FILE *sourceFile;
	char line[1024];
	char *p,*q;
	char *t;
	
	// initialization
	samplingRate=0;
	i=1;
	p=NULL;
	q=NULL;

	if((sourceFile=fopen(inputFile,"rb"))!=NULL)
	{

		t=fgets(line,sizeof(char)*1024,sourceFile);
		while(t!=feof)
		{
			if((p=strstr(line,"cacheTimePerFrame"))!=NULL)
				break;

			t=fgets(line,sizeof(char)*1024,sourceFile);
		}
	}
	if(p!=NULL)
	{
		// operation to get the sampling rate value
	}
	fclose(sourceFile);

	return samplingRate;
}

void getSourceCacheFileName(char **sourceCacheFileName,const char *sourceFile, const int  frame)
{
	char *path, *name, *extension,*extensionFileName;
	int sequenceFileLength;

	extensionFileName=NULL;
	path=NULL;
	name=NULL;
	extension=NULL;

	if (*sourceCacheFileName!=NULL)
		free(*sourceCacheFileName);

		sequenceFile(&extensionFileName, frame, &sequenceFileLength);
		getFileInfo(sourceFile, &path, &name, &extension);
		*sourceCacheFileName=(char*)malloc(sizeof(char)*(strlen(path)+strlen(name)+strlen(extensionFileName)+FRAMESUBFIXLENGTH+MAYACACHEEXTENSIONLENGTH+1));
		memset(*sourceCacheFileName,'\0',sizeof(char));
		strcat(*sourceCacheFileName,path);
		strcat(*sourceCacheFileName,name);
		strcat(*sourceCacheFileName,FRAMESUBFIX);
		strcat(*sourceCacheFileName,extensionFileName);
}



BOOL _MultiFileToSingleFileConverter(char *sourceFile, char * destinationFile)
{
	FILE *sourceCacheFile, *destinationCacheFile; 
	char *sourceCacheFileName, *destinationCacheFileName;
	Header intestazione;
	Header header;
	OFXFBlock ofxfBlock;
	SFBlock sfBlock;
	int sampligRate;
	unsigned int srStep;
	int sequenceIndex;
	int dataToRead;
	char *data;
	int temp;

	BOOL fileConverted=TRUE;
	sequenceIndex=0;
	srStep=0;
	sourceCacheFileName=NULL;
	destinationCacheFileName=NULL;

	// convert the xml file and starting cache convertion
	if(xmlConverter(sourceFile,destinationFile,ONEFILEPERFRAME,ONEFILE))
	{
		// open destination file d:\\temp\\destinationFile.mc
		//if((destinationCacheFile=fopen(destinationFile,"wb"))==NULL)
		//getSourceCacheFileName(&destinationCacheFileName,destinationFile,-1);
		if((destinationCacheFile=fopen("d:\\temp\\destinationFile.mc","wb"))==NULL)	
			return FALSE;

		
		//sampligRate=swapint(getSamplingRate(destinationFile));
		//sampligRate=MAYATICK/swapint(250);
		temp=250;
		getSourceCacheFileName(&sourceCacheFileName,sourceFile,sequenceIndex);

		memcpy(header.format, FOR4, sizeof(header.format));
		header.length = swapint(40);
		memcpy(header.cacheVersion, CACHEVERSION, sizeof(header.cacheVersion));
		header.separator1 = swapint(4);
		memcpy(header.version, "0.1", sizeof(header.version));
		memcpy(header.stim, STIM, sizeof(header.stim)); 
		memcpy(header.etim, ETIM, sizeof(header.etim));
		header.stimFirstPart = swapint(4);
		header.etimFirstPart = swapint(4);
		header.stimSecondPart = swapint(0);	// compute after the value
		header.etimSecondPart = swapint(1);  // compute after the value
		fwrite(&header, sizeof(header), 1, destinationCacheFile);

		// while sourcefiles availlables....
		while ((sourceCacheFile=fopen(sourceCacheFileName,"rb"))!=NULL)
		{
			fread(&intestazione,sizeof(Header),1,sourceCacheFile);
			fread(&ofxfBlock,sizeof(OFXFBlock),1,sourceCacheFile);
			
			dataToRead=swapint(ofxfBlock.blockLength)-4;
			data=(char*)malloc(sizeof(char)*dataToRead);
			fread(data,sizeof(char),dataToRead,sourceCacheFile);
			sfBlock.blockLength=swapint(16+dataToRead);
			memcpy(sfBlock.blockTag, MYCH, sizeof(sfBlock.blockTag));
			sfBlock.currentTimeTicks=swapint(sequenceIndex*temp);
			sfBlock.timeDataLength=swapint(4);
			memcpy(sfBlock.format, FOR4, sizeof(sfBlock.format));
			memcpy(sfBlock.time, TIME, sizeof(sfBlock.time));
			fwrite(&sfBlock,sizeof(SFBlock),1,destinationCacheFile);
			
			//write data on target file (all the channel at the actual frame )
			fwrite(data,sizeof(char),dataToRead,destinationCacheFile);
			
			// building new cache file name to search and update the "time"
			sequenceIndex++;

			// closing file
			fclose(sourceCacheFile);
			if (sourceCacheFileName!=NULL)
			{
				free(sourceCacheFileName);
				sourceCacheFileName=NULL;
			}

			// open new file
			getSourceCacheFileName(&sourceCacheFileName,sourceFile,sequenceIndex);
			fflush(destinationCacheFile);
			free(data);
		}

		if((sourceCacheFile=fopen(sourceFile,"rb"))==NULL)
			return FALSE;
		fclose(destinationCacheFile);
		fclose(sourceCacheFile);
	}
	else
		fileConverted=FALSE;

	return fileConverted;
};

