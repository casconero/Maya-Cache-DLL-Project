#include "XmlWriter.h"

void printXml(Channel *channel, int cacheId)
{
	int i;
	Info *thisInfo = getInfo(cacheId);
	thisInfo->mayaXMLFile=fopen(thisInfo->xmlFileName, "wb");
	//info.mayaXMLFile=fopen(info.xmlFileName, "wb");
	//setvbuf(info.mayaXMLFile, info.xmlChannelBuffer, _IOFBF, BUFFERLENGTH);
	setvbuf(thisInfo->mayaXMLFile, thisInfo->xmlChannelBuffer, _IOFBF, BUFFERLENGTH);

	//if(info.mayaXMLFile!=NULL)
	if(thisInfo->mayaXMLFile!=NULL)
	{
		//fprintf(info.mayaXMLFile,"<?xml version=\"1.0\"?>\n");
		fprintf(thisInfo->mayaXMLFile,"<?xml version=\"1.0\"?>\n");
		//switch (info.cacheFormat)
		switch (thisInfo->cacheFormat)
		{
		case ONEFILE:
			//fprintf(info.mayaXMLFile,"<Autodesk_Cache_File>\n\t<cacheType Type=\"OneFile\" Format=\"mcc\"/>\n");
			fprintf(thisInfo->mayaXMLFile,"<Autodesk_Cache_File>\n\t<cacheType Type=\"OneFile\" Format=\"mcc\"/>\n");
			break;
		case ONEFILEPERFRAME:
			//fprintf(info.mayaXMLFile,"<Autodesk_Cache_File>\n\t<cacheType Type=\"OneFilePerFrame\" Format=\"mcc\"/>\n");
			fprintf(thisInfo->mayaXMLFile,"<Autodesk_Cache_File>\n\t<cacheType Type=\"OneFilePerFrame\" Format=\"mcc\"/>\n");
			break;
		}
		//fprintf(info.mayaXMLFile,"\t<time Range=\"%d-%d\"/>\n\t<cacheTimePerFrame TimePerFrame=\"%d\"/>\n", info.start, info.end, info.mayaTPF);
		//fprintf(info.mayaXMLFile,"\t<cacheVersion Version=\"2.0\"/>\n");
		fprintf(thisInfo->mayaXMLFile,"\t<time Range=\"%d-%d\"/>\n\t<cacheTimePerFrame TimePerFrame=\"%d\"/>\n", thisInfo->start, thisInfo->end, thisInfo->mayaTPF);
		fprintf(thisInfo->mayaXMLFile,"\t<cacheVersion Version=\"2.0\"/>\n");

		/*
		for (i=0;i<info.nExtras;i++)
			fprintf(info.mayaXMLFile,"\t<extra>%s</extra>\n", info.extras[i]);
		
		fprintf(info.mayaXMLFile,"\t<Channels>\n");
		*/
		for (i=0;i<thisInfo->nExtras;i++)
			fprintf(thisInfo->mayaXMLFile,"\t<extra>%s</extra>\n", thisInfo->extras[i]);

		fprintf(thisInfo->mayaXMLFile,"\t<Channels>\n");
		writeXmlChannel(channel,cacheId);
	}
	else
		printf("Unable to open %s file in write mode\n",thisInfo->xmlFileName);
		//printf("Unable to open %s file in write mode\n",info.xmlFileName);
}

void writeXmlChannel(Channel *channel,int cacheId)
{
	Info *thisInfo =getInfo(cacheId);
	char *channelType;
	int i,number = 0;
	int j=0;
    
	//if(info.mayaXMLFile!=NULL)
	if(thisInfo->mayaXMLFile!=NULL)
		for(i=0;i<CACHENUMBEROFCHANNELS;i++)
		{
			if(channel[i].type==DBLA)
				channelType = DOUBLEARRAY;
			else
				channelType = FLOATVECTORYARRAY;
			if (channel[i].enabled)
				//fprintf(info.mayaXMLFile,"\t\t<channel%d ChannelName=\"%s\" ChannelType=\"%s\" ChannelInterpretation=\"%s\" SamplingType=\"Regular\" SamplingRate=\"%d\" StartTime=\"%d\" EndTime=\"%d\"/>\n",j++,channel[i].name, channelType, names[i], info.mayaTPF, info.start, info.end);
				fprintf(thisInfo->mayaXMLFile,"\t\t<channel%d ChannelName=\"%s\" ChannelType=\"%s\" ChannelInterpretation=\"%s\" SamplingType=\"Regular\" SamplingRate=\"%d\" StartTime=\"%d\" EndTime=\"%d\"/>\n",j++,channel[i].name, channelType, names[i], thisInfo->mayaTPF, thisInfo->start, thisInfo->end);
		}
	else
		printf("info.mayaXMLFile=NULL\n");
}

void closeXmlFile(int cacheId)
{
	
	Info *thisInfo = getInfo(cacheId);

	if(thisInfo->mayaXMLFile!=NULL)
	{
		fflush(thisInfo->mayaXMLFile);
		fprintf(thisInfo->mayaXMLFile,"\t</Channels>\n</Autodesk_Cache_File>\n");
		fclose(thisInfo->mayaXMLFile);
	}
	isXmlFinalized[cacheId-1]=FALSE;
}
