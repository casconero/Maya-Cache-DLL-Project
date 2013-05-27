#define DLL

#include <CacheTranscoding.h>
#include <MayaCache.h>
#include <XmlWriter.h>
#include <FileUtility.h>

 GENERICSWAP(double,unsigned long long)		// swap for double variables: swapdouble(unsigned long long d)
 GENERICSWAP(float,unsigned int)			// swap for float variables: swapfloat(unsigned int d)
 GENERICSWAP(int,unsigned int)				// swap for int variables:  swapint(unsigned int d)

  //utility functions
 int selectCache(int id);
 void makeName(CHANNELTYPE type, char **channelName,int cacheId);
 BOOL fileExists(const char * filename);
 void writeMayaCacheHeader(int cacheId);
 void writeMayaCacheChannel(CHANNELTYPE cType,int cacheId);
 void writeMayaCacheBlock(int cacheId);

// to do. modificare in maniera tale che il numero di canali sia definibile a runtime (devo aggiungere i canali definiti dagli utenti)

static Info *infos;
static int infosSize;
 
 
 // not exposed functions
 Info *getInfo(int cacheId)
 {
	 int  index=selectCache(cacheId);
	 return &infos[index];
 }
 
 BOOL SingleFileToMultiFileConverter(char *sourceFile, char * destinationFile)
 {
	 return _SingleFileToMultiFileConverter(sourceFile, destinationFile);
	 
 }

 BOOL MultiFileToSingleFileConverter(char *sourceFile, char * destinationFile)
 {
	 return _MultiFileToSingleFileConverter(sourceFile, destinationFile);
 };

 void enableChannel(CHANNELTYPE channelActive, ENABLEDISABLED ea, int cacheId)
 {
	 int  index=selectCache(cacheId);
	 infos[index].mayaChannels[channelActive].enabled=ea;
	 infos[index].mayaChannels[channelActive].defineByUser=FALSE;
	 infos[index].mayaChannels[channelActive].elementsD=NULL;
	 infos[index].mayaChannels[channelActive].elementsF=NULL;
	 if (ea==ENABLED)
	 {
		infos[index].mayaChannels[channelActive].numberOfElements=infos[index].numberOfElements;
		// assigning name
		makeName(channelActive, &infos[index].mayaChannels[channelActive].name,cacheId);
		switch(channelActive) 
		{
			case IDCHANNEL:
			case COUNTCHANNEL:
			case BIRTHTIMECHANNEL:
			case MASSCHANNEL:
			case AGECHANNEL:
			case OPACITYPPCHANNEL:
			case RADIUSPPCHANNEL:
			case FINALLIFESPANPPCHANNEL:
				infos[index].mayaChannels[channelActive].type = DBLA;
				infos[index].mayaChannels[channelActive].numberOfComponents = 1;
				break;
			case RGBPPCHANNEL:
			case POSITIONCHANNEL:
			case VELOCITYCHANNEL:
			case ACCELERATIONCHANNEL:
			case WORLDPOSITIONCHANNEL:
			case WORLDVELOCITYCHANNEL:
			case WORLDVELOCITYINOBJECTSPACECHANNEL:
				infos[index].mayaChannels[channelActive].numberOfComponents = 3;
				infos[index].mayaChannels[channelActive].type=FVCA;
				break;
			case LIFESPANPPCHANNEL:
				infos[index].mayaChannels[channelActive].numberOfComponents = 3;
				infos[index].mayaChannels[channelActive].type = DBLA;
				break;
		};
		infos[index].numberOfChannels++;
	 } 
	 else
	 {

		infos[index].numberOfChannels--;
		infos[index].mayaChannels[channelActive].numberOfElements=0;
		infos[index].mayaChannels[channelActive].numberOfComponents = 0;
		infos[index].mayaChannels[channelActive].type=FVCA;
		free(infos[index].mayaChannels[channelActive].elementsD);
		free(infos[index].mayaChannels[channelActive].elementsF);
		free(infos[index].mayaChannels[channelActive].name);
		infos[index].mayaChannels[channelActive].elementsD=NULL;
		infos[index].mayaChannels[channelActive].elementsF=NULL;
	 }
}

 void assignChannelValues(CHANNELTYPE channelActive, void *sourceValues, int cacheId)
 {
	int  index=selectCache(cacheId);
	if (infos[index].mayaChannels[channelActive].type == FVCA)
	{
		infos[index].mayaChannels[channelActive].elementsF=(float *)sourceValues;
		infos[index].mayaChannels[channelActive].elementsD=NULL;
	}
	else
	{
		infos[index].mayaChannels[channelActive].elementsD=(double*)sourceValues;
		infos[index].mayaChannels[channelActive].elementsF=NULL;
	}
 }
 
 int init(char *particleSysName,char *fileName, CACHEFORMAT cacheFormat,int nExtraChannels, int numberOfElements, unsigned int fps, double start, double end,char *extra[], int nE)
 {
	char *filePath;
	char *fileName2;
	char *fileExtension;
	char *mcName, *xmlName;
	
	int i=0,padding=1,temp=0;
	
	temp=(int)(end-start)*fps;

	if(infos==NULL)
	{
		infosSize=1;
		infos=(Info*)malloc(sizeof(Info));
		isXmlFinalized=(BOOL*)malloc(sizeof(BOOL));
	}
	else
	{
		infosSize++;
		infos=realloc(infos,infosSize*sizeof(Info));
		isXmlFinalized=realloc(isXmlFinalized,infosSize*sizeof(BOOL));
	}
	infos[infosSize-1].id=infosSize;

	infos[infosSize-1].mayaChannels=(Channel*)malloc(sizeof(Channel)*(CACHENUMBEROFCHANNELS+nExtraChannels));
	filePath=NULL;
	fileName2=NULL;
	fileExtension=NULL;
	
	infos[infosSize-1].extras=(char**)malloc(nE*sizeof(char*));
	infos[infosSize-1].nExtras=nE;
	
	for (i=0;i<nE;i++)
	{	
		infos[infosSize-1].extras[i]=(char*)malloc(sizeof(char)*strlen(extra[i]));
		strcpy(infos[infosSize-1].extras[i],extra[i]);
	}

	infos[infosSize-1].numberOfChannels=0;
	infos[infosSize-1].nUserDefinedChannel=nExtraChannels;
	infos[infosSize-1].particleSysName=particleSysName;
	infos[infosSize-1].numberOfElements=numberOfElements;
	infos[infosSize-1].startFrame=(int)floor(start*fps);			
	infos[infosSize-1].currentFrame=infos[infosSize-1].startFrame;
	infos[infosSize-1].frameIncrement=1;

	for(i=0;i<CACHENUMBEROFCHANNELS;i++)
		infos[infosSize-1].mayaChannels[i].enabled=DISABLED;

	while (temp/10>1)
	{
		padding++;
		temp=temp/10;
	}

	// adding extension ".mc"
	mcName = (char*)malloc(sizeof(char)*(strlen(fileName)+padding+MAYACACHEEXTENSIONLENGTH+1));
	strcpy(mcName, fileName);
	strcat(mcName, MAYACACHEEXTENSION);
	infos[infosSize-1].mcFileName=mcName;

	infos[infosSize-1].mayaCacheFileName=(char*)malloc(sizeof(char)*(strlen(fileName)));
	strcpy(infos[infosSize-1].mayaCacheFileName, fileName);
	
	printf("Enabling %s in write mode\n",infos[infosSize-1].mcFileName);
	
	// adding extension ".xml"
	xmlName = (char*)malloc(sizeof(char)*(strlen(fileName)+XMLEXTENSIONLENGTH+1));
	strcpy(xmlName, fileName);
	strcat(xmlName, XMLEXTENSION);
	infos[infosSize-1].xmlFileName=xmlName;

	printf("Enabling %s in write mode \n",infos[infosSize-1].xmlFileName);

	infos[infosSize-1].cacheFormat=cacheFormat;
	infos[infosSize-1].fps=fps;											// [frame/sec]
	infos[infosSize-1].duration=(int)(fps*(end-start));					// [frame]
	infos[infosSize-1].mayaTPF=MAYATICK/fps;							// [mayaticks/frame]
	infos[infosSize-1].start=(int)floor(start*fps)*infos[infosSize-1].mayaTPF;		// [mayaticks]
	infos[infosSize-1].end=infos[infosSize-1].duration*infos[infosSize-1].mayaTPF+infos[infosSize-1].start;		// [mayaticks]

	if (infos[infosSize-1].cacheFormat==ONEFILE)
	{
		// setting output buffer, buffering mode, size 
		infos[infosSize-1].mayaMCFile=fopen(infos[infosSize-1].mcFileName, "wb");
		//setvbuf(infos[infosSize-1].mayaMCFile, infos[infosSize-1].mcChannelBuffer, _IOFBF, BUFFERLENGTH);
		writeMayaCacheHeader(infosSize);
	}

	isXmlFinalized[infosSize-1]=FALSE;
	return infos[infosSize-1].id;
 }

void closeMayaCacheFile(int cacheId)
{
	int  index=selectCache(cacheId);

	if(infos[index].mayaMCFile!=NULL)
	{
		fflush(infos[index].mayaMCFile);
		fclose(infos[index].mayaMCFile);
		if (infos[index].cacheFormat == ONEFILE)
			printf("File '%s' was successfully closed and saved\n", infos[index].mcFileName);
	}
	else
		printf("Unable to save NCache %s file\n", infos[index].mcFileName);
}

// header 
void writeMayaCacheHeader(int cacheId) 
{
	Header header;
	int startTime=0,endTime=0;
	int  index=selectCache(cacheId);

	if(infos[index].mayaMCFile==NULL)
		printf("Unable to open %s file in write mode\n",infos[index].mcFileName);
	else
	{
		memcpy(header.format, FOR4, sizeof(header.format));
		header.length = swapint(40);
		memcpy(header.cacheVersion, CACHEVERSION, sizeof(header.cacheVersion));
		header.separator1 = swapint(4);
		memcpy(header.version, "0.1", sizeof(header.version));
		memcpy(header.stim, STIM, sizeof(header.stim)); 
		memcpy(header.etim, ETIM, sizeof(header.etim));
		header.stimFirstPart = swapint(4);
		header.etimFirstPart = swapint(4);
		if (infos[index].cacheFormat==ONEFILE)
		{
			header.stimSecondPart = swapint(startTime);	
			header.etimSecondPart = swapint(endTime);
		}
		else
		{
			header.stimSecondPart = swapint(infos[index].currentFrame*infos[index].mayaTPF);
			header.etimSecondPart = header.stimSecondPart;
		}
		fwrite(&header, sizeof(header), 1, infos[index].mayaMCFile);
		//fflush(infos[index].mayaMCFile);
	}
}

// channel
void writeMayaCacheChannel(CHANNELTYPE cType, int cacheId)
{
	unsigned int  dim, swappedDim, padding, size, nElements, i, channelArrayLength, dataTypeSize, arrayElements, temp, four;
	long long int channelLenght=0, tempLong;
	float * pFloat;
	double * pDouble;
	char type[4];
	char *paddingString;
	int  index=selectCache(cacheId);

	dim = (unsigned int)strlen(infos[index].mayaChannels[cType].name);
	padding = 4 - dim%4;
	swappedDim = swapint(dim+1);
	four = swapint(4);
	size = swapint(infos[index].mayaChannels[cType].numberOfElements);
	
	if(infos[index].mayaChannels[cType].type==FVCA)
	{
		pFloat = infos[index].mayaChannels[cType].elementsF;
		channelArrayLength = 3 * 4 * infos[index].mayaChannels[cType].numberOfElements;
		strncpy(type, FVCACHANNEL, 4);
		dataTypeSize = 4;
		arrayElements = 3 * infos[index].mayaChannels[cType].numberOfElements;
	}
	else
	{
		pDouble = infos[index].mayaChannels[cType].elementsD;
		strncpy(type, DBLACHANNEL, 4);
		dataTypeSize = 8;

		if (cType == COUNTCHANNEL)
		{
			channelArrayLength = 8;
			arrayElements = 1;
			size = swapint(1);
		}
		else
		{
			arrayElements = infos[index].mayaChannels[cType].numberOfElements;
			channelArrayLength = 8 * arrayElements;
		}
	}

	channelArrayLength = swapint(channelArrayLength);
	nElements = swapint(arrayElements);
	paddingString = (char*)calloc(padding, sizeof(char));
	fwrite(CHNM, 4*sizeof(char), 1, infos[index].mayaMCFile);
	fwrite(&swappedDim, sizeof(int), 1, infos[index].mayaMCFile);
	fwrite(infos[index].mayaChannels[cType].name, dim, 1, infos[index].mayaMCFile);
	fwrite(paddingString, padding, 1, infos[index].mayaMCFile);
	fwrite(SIZE, 4*sizeof(char), 1, infos[index].mayaMCFile);
	fwrite(&four, sizeof(int), 1, infos[index].mayaMCFile);
	fwrite(&size, sizeof(int), 1, infos[index].mayaMCFile);
	fwrite(type, 4*sizeof(char), 1, infos[index].mayaMCFile);
	fwrite(&channelArrayLength, sizeof(int), 1, infos[index].mayaMCFile);
		
	//writing elements
	for(i=0; i<arrayElements; i++)
	{
		//swap type + write
		if(infos[index].mayaChannels[cType].type==FVCA)
		{
			temp=swapfloat(pFloat[i]);
			fwrite(&temp, sizeof(int), 1, infos[index].mayaMCFile);
		}
		else
		{
			tempLong = swapdouble(pDouble[i]);
			fwrite(&tempLong, sizeof(long long int), 1, infos[index].mayaMCFile);
		}
	}
	free(paddingString);
}

// data block
void writeMayaCacheBlock(int cacheId)
{
	// single data block writing, 
	// each Block start with "FOR4"
	// each block contains:
	// MYCH (Group)	
	// TIME		// Time (int)

	unsigned int time, dimB, timeVar, nChannels=0, dataLength=0, dataSize=0;
	int padding,i;
	int  index=selectCache(cacheId);
	timeVar = swapint(4);
	
	time = swapint(infos[index].currentFrame*MAYATICK/infos[index].fps);
	fwrite(FOR4, 4*sizeof(char), 1, infos[index].mayaMCFile);

	// computing the block length
	for(i=0;i<CACHENUMBEROFCHANNELS;i++)
	{
		int size=0;
		dataSize = 0;
		if (infos[index].mayaChannels[i].enabled)
		{
			//printf("%s\n",infos[index].mayaChannels[i].name);
			padding=4-strlen(infos[index].mayaChannels[i].name)%4;
			dataLength += (unsigned int)strlen(infos[index].mayaChannels[i].name) + padding;
			if(infos[index].mayaChannels[i].type==FVCA)
				dataSize = 3 * 4 * infos[index].mayaChannels[i].numberOfElements;
			else
				if (i==COUNTCHANNEL)
					dataSize = 8;
				else
					dataSize = 8* infos[index].mayaChannels[i].numberOfElements;
			
			dataLength += dataSize;
		}
	}
	
	if (infos[index].cacheFormat==ONEFILE)
		dimB = SINGLEFILEBLOCKFIXEDDIM + CHANNELFIXEDDIM * infos[index].numberOfChannels + dataLength;	
	else
		dimB = MULTIFILEBLOCKFIXEDDIM + CHANNELFIXEDDIM * infos[index].numberOfChannels + dataLength;	

	dimB = swapint(dimB);
	fwrite(&dimB, sizeof(unsigned int), 1, infos[index].mayaMCFile);
	fwrite(MYCH, 4*sizeof(char), 1, infos[index].mayaMCFile);
	if (infos[index].cacheFormat==ONEFILE)
	{
		fwrite(TIME, 4*sizeof(char), 1, infos[index].mayaMCFile);
		fwrite(&timeVar, sizeof(int), 1, infos[index].mayaMCFile);
		fwrite(&time, sizeof(int), 1, infos[index].mayaMCFile);
	}

	for(i=0;i<CACHENUMBEROFCHANNELS;i++)
		if (infos[index].mayaChannels[i].enabled)
			writeMayaCacheChannel(i,cacheId);
		
	fflush(infos[index].mayaMCFile);
	if(isXmlFinalized[index]==FALSE)
	{
		printXml(infos[index].mayaChannels,cacheId);
		closeXmlFile(cacheId);
		isXmlFinalized[index]=TRUE;
	}
}

// entry point for saving data in maya cache file format (support "one file per frame" and "one file" options)
void mayaCache(int cacheId)
{
	char *mcName;
	char *subfix;
	int sflenght,fps;
	int  index=selectCache(cacheId);

	if (infos[index].cacheFormat==ONEFILE)
		writeMayaCacheBlock(cacheId);
	else
	{
	    fps=infos[index].currentFrame;
		sflenght=1;
		while (fps>1)
		{
			sflenght++;
			fps=fps/10;
		}
		subfix = (char*)malloc(sizeof(char)*(strlen(infos[index].mayaCacheFileName)+sflenght));
		itoa(infos[index].currentFrame,subfix,10);
		
		// adding extension ".mc"
		// "Frame"---->5
		// ADDING eos "\0"
		mcName = (char*)malloc(sizeof(char)*(strlen(infos[index].mayaCacheFileName)+sflenght+MAYACACHEEXTENSIONLENGTH+FRAMELENGTH +1));
		strcpy(mcName, infos[index].mayaCacheFileName);
		strcat(mcName, "Frame");
		strcat(mcName, subfix);
		strcat(mcName, MAYACACHEEXTENSION);
		if (infos[index].mcFileName!=NULL)
			free(infos[index].mcFileName);
		infos[index].mcFileName=mcName;
		
		
		// setting output buffer, buffering mode, size 
		infos[index].mayaMCFile=fopen(infos[index].mcFileName, "wb");
		//setvbuf(infos[index].mayaMCFile, infos[index].mcChannelBuffer, _IOFBF, BUFFERLENGTH);

		writeMayaCacheHeader(cacheId);
				
		writeMayaCacheBlock(cacheId);
		if(infos[index].mayaMCFile!=NULL)
			closeMayaCacheFile(cacheId);
		else
			printf("Maya NCache file '%s' not saved!\n", infos[index].mcFileName);
	}

	infos[index].currentFrame+=infos[index].frameIncrement;
}

void makeName(CHANNELTYPE type, char **channelName,int cacheId)
{
	int dim;
	int additionalDigit=0;
	int digit=0;
	int  index=selectCache(cacheId);
	char *underscore={"_"};
	switch(type)
	{
		case IDCHANNEL:
			dim=4;
			break;
		case COUNTCHANNEL:
			dim=7;
			break;
		case BIRTHTIMECHANNEL:
			dim=11;
			break;
		case POSITIONCHANNEL:
			dim=10;
			break;
		case LIFESPANPPCHANNEL:
			dim=12;
			break;
		case FINALLIFESPANPPCHANNEL:
			dim=17;
			break;
		case VELOCITYCHANNEL:
			dim=10;
			break;
		case ACCELERATIONCHANNEL:
			dim=14;
			break;
		case WORLDPOSITIONCHANNEL:
			dim=15;
			break;
		case WORLDVELOCITYCHANNEL:
			dim=15;
			break;
		case WORLDVELOCITYINOBJECTSPACECHANNEL:
			dim=28;
			break;
		case MASSCHANNEL:
			dim=6;
			break;
		case AGECHANNEL:
			dim=5;
			break;
		case RGBPPCHANNEL:
			dim=7;
			break;
		case OPACITYPPCHANNEL:
			dim=11;
			break;
		case RADIUSPPCHANNEL:
			dim=10;
			break;
		default:
			// user defined channel
			/* USERDEFINEDCHANNEL */
			dim=13;
			break;
	}
	*channelName=(char*)calloc(sizeof(infos[index].particleSysName)+dim, sizeof(char));
	strcpy(channelName[0], infos[index].particleSysName);
	strcat(channelName[0], underscore);
	if (type!=USERDEFINEDCHANNEL)
		strcat(channelName[0], names[type]);
	else
	{
		// user defined channel
		strcat(channelName[0], "USERDEFINED");
	}
	
}

int selectCache(int id)
{
	int index=0;
	while(index<infosSize)
	{
		if (infos[index].id==id)
			return index;

		index++;
	}
};

BOOL fileExists(const char * filename)
{
	FILE * file;
	if ( file= fopen(filename, "r"))
	{
		fclose(file);
		return TRUE;
	}
	return FALSE;
}

void deleteFile(int cacheId)
{
	int  index=selectCache(cacheId);

	// closing the streams
	closeMayaCacheFile(cacheId);

	// deleting mc file
	if(fileExists(infos[index].mcFileName))
		remove(infos[index].mcFileName);

	// deleting xml file
	if(fileExists(infos[index].xmlFileName))
		remove(infos[index].xmlFileName);
}
/*****************************************************************************************************************************************************************/

/*****************************Cache transforming operation********************************************************************************************************/




/*****************************************************************************************************************************************************************/
unsigned int DLL_EXPORT getDuration(int cacheId)
{
	int  index=selectCache(cacheId);
	return infos[index].duration;
}

int DLL_EXPORT getStartFrame(int cacheId)
{
	int  index=selectCache(cacheId);
	return infos[index].startFrame;
}

int DLL_EXPORT getFrameIncrement(int cacheId)
{
	int  index=selectCache(cacheId);
	return infos[index].frameIncrement;
}

