#include "CacheTranscoding.h"

// utilities funcstions
BOOL xmlConverter(char *xmlSourceFile, char *xmlDestinationFile,CACHEFORMAT sourceFormat, CACHEFORMAT destinationFormat);

void retrieveFileName(char * fileNameInput, char ** fileNameOutput, int *sourceFileLength);

void sequenceFile(char **sequence, int frame, int *length);

void getSourceCacheFileName(char **sourceCacheFileName,const char sourceFile, const int  frame);


// utilities functions implementation 
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


// callable method
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


	char *mcpath,*mcName,*mcExt;
	char mc[4] =".mc";
	BOOL fileConverted = TRUE;
	sequenceIndex = 0;
	srStep = 0;
	sourceCacheFileName = NULL;
	destinationCacheFileName = NULL;
	mcpath = NULL;
	mcName = NULL;
	mcExt = NULL;

	
	// convert the xml file and starting cache convertion
	if(xmlConverter(sourceFile,destinationFile,ONEFILEPERFRAME,ONEFILE))
	{
		// getting the mc file name and path
		getFileInfo(destinationFile,&mcpath,&mcName,&mcExt);
		buildNameFile(mcpath, mcName ,NULL, ".mc",&destinationCacheFileName);

		if((destinationCacheFile = fopen(destinationCacheFileName,"wb"))==NULL)	
			return FALSE;

		// opening the first source cache file 
		sampligRate = swapint(getSamplingRate(destinationFile));
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
		while ((sourceCacheFile = fopen(sourceCacheFileName,"rb"))!=NULL)
		{
			fread(&intestazione,sizeof(Header),1,sourceCacheFile);
			fread(&ofxfBlock,sizeof(OFXFBlock),1,sourceCacheFile);
			
			dataToRead = swapint(ofxfBlock.blockLength)-4;
			data=(char*)malloc(sizeof(char)*dataToRead);
			fread(data,sizeof(char),dataToRead,sourceCacheFile);
			sfBlock.blockLength = swapint(16+dataToRead);
			memcpy(sfBlock.blockTag, MYCH, sizeof(sfBlock.blockTag));
			sfBlock.currentTimeTicks = swapint(sequenceIndex*sampligRate);
			sfBlock.timeDataLength = swapint(4);
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

			// open the new file 
			getSourceCacheFileName(&sourceCacheFileName,sourceFile,sequenceIndex);
			fflush(destinationCacheFile);
			free(data);
		}

		if((sourceCacheFile = fopen(sourceFile,"rb"))==NULL)
			return FALSE;
		fclose(destinationCacheFile);
		fclose(sourceCacheFile);
	}
	else
		fileConverted = FALSE;

	return fileConverted;
};

BOOL _SingleFileToMultiFileConverter(char *sourceFile, char * destinationFile)
{
	FILE *sourceCacheFile, *destinationCacheFile; 
	char *sourceCacheFileName, *destinationCacheFileName;
	char *mcpath,*mcName,*mcExt;
	char*buffer;
	Header header;
	OFXFBlock block;
	SFBlock sfBlock;
	int sequenceIndex;
	int channelsLength;
	int mayaTpf;

	// initialization
	sequenceIndex = 0;
	channelsLength = 0;
	mcpath = NULL;
	mcName = NULL;
	mcExt = NULL;
	buffer = NULL;
	destinationCacheFileName = NULL;

	if(xmlConverter(sourceFile,destinationFile,ONEFILE,ONEFILEPERFRAME))
	{
		// getting the source mc file name and path
		getFileInfo(sourceFile,&mcpath,&mcName,&mcExt);
		buildNameFile(mcpath, mcName ,NULL, ".mc",&sourceCacheFileName);
		
		//open the source mc single file
		if((sourceCacheFile=fopen(sourceCacheFileName,"rb"))==NULL)	
			return FALSE;

		mayaTpf = getSamplingRate(sourceFile);

		// reading the header field
		fread(&header,sizeof(Header),1,sourceCacheFile);
		
		// read the channels dimension (first time)
		fread(&sfBlock,sizeof(SFBlock),1,sourceCacheFile);

		// read one frame at time and save the frame in the new file
		// test end of file
		while(!feof(sourceCacheFile))
		{
			header.stimSecondPart = swapint(sequenceIndex*mayaTpf);
			header.etimSecondPart = header.stimSecondPart;

			if (buffer!=NULL)
				free(buffer);

			//number of byte to read and saving on the output file
			channelsLength = swapint(sfBlock.blockLength)-16;
			buffer = (char*)malloc(sizeof(char)*channelsLength);
			fread(buffer,sizeof(char),channelsLength,sourceCacheFile);

			block.blockLength = swapint(channelsLength+4);
			memcpy(block.format, FOR4, sizeof(block.format));
			memcpy(block.blockTag, MYCH, sizeof(block.blockTag));

			// saving file
			getSourceCacheFileName(&destinationCacheFileName,destinationFile,sequenceIndex);
			if((destinationCacheFile = fopen(destinationCacheFileName,"wb"))==NULL)	
				return FALSE;

			fwrite(&header, sizeof(header), 1, destinationCacheFile);
			fwrite(&block, sizeof(OFXFBlock), 1, destinationCacheFile);
			fwrite(buffer, sizeof(char), channelsLength, destinationCacheFile);
			fflush(destinationCacheFile);
			fclose(destinationCacheFile);

			sequenceIndex++;

			// read the channels dimension for the sequenceIndex step
			fread(&sfBlock,sizeof(SFBlock),1,sourceCacheFile);
		}
	}

	return TRUE;
}

