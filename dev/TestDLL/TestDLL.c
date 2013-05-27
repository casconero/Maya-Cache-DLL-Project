#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MayaCache.h>

//example: how to use the MayaCache library
int main()
{
	// 2 particles moving in a circular path
	// first one, rotates around X axis clockwise
	// second one, rotates around Y axis clockwise
	// the example will save the particles' positions, velocities and colors

	double  *id, *birthtime,*lifespanPP,*finalLifespanPP;
	float *position,*velocity,*color;
	double count,start,end;
	unsigned long numberOfElements=2;
	float r1,r2,angle1,angle2,delta;
	int j,nExtras,cache1,cache2;
	unsigned int fps;
	CACHEFORMAT cachingMethod;
	char *extras[4];	// extra parameters list
	nExtras=4;			// numer of extra parameters

	fps=24;			// frame per seconds
	start=0;		// start at time "0"
	end=2;			// simulation lenght "2" (in seconds) 
	r1=5;			// first particle rotation radius
	r2=7;			// second particle rotation radius
	angle1=0;		// first particle starting angle
	angle2=90;		// second particle starting angle

	// Extra information for the cached simulation; 
	// info will be displayed in the Cache Description section 
	// of the cacheFile node.
	extras[0]="saving path";						// path to maya file where simulation was done
	extras[1]="maya 2011 x64";						// maya version
	extras[2]="100cells";							// owner
	extras[3]="NCache Info for nParticleShape1";	// 


	// Initializing channels options, simulation parameters and saving method type
	//cachingMethod=ONEFILE;//ONEFILEPERFRAME; //ONEFILE;	//

	cache1=init("nParticleShape1","d://temp//experimental//ONEFILE",ONEFILE,0,numberOfElements,fps,start,end,extras,nExtras);
	cache2=init("nParticleShape2","d://temp//experimental//TestSenzaVelocity",ONEFILEPERFRAME,0,numberOfElements,fps,start,end,extras,nExtras);

	enableChannel(IDCHANNEL, ENABLED,cache1);
	enableChannel(COUNTCHANNEL, ENABLED,cache1);
	enableChannel(BIRTHTIMECHANNEL, ENABLED,cache1);
	enableChannel(POSITIONCHANNEL, ENABLED,cache1);
	enableChannel(LIFESPANPPCHANNEL, ENABLED,cache1);
	enableChannel(FINALLIFESPANPPCHANNEL, ENABLED,cache1);
	enableChannel(VELOCITYCHANNEL, ENABLED,cache1);
	enableChannel(RGBPPCHANNEL, ENABLED,cache1);

	enableChannel(IDCHANNEL, ENABLED,cache2);
	enableChannel(COUNTCHANNEL, ENABLED,cache2);
	enableChannel(BIRTHTIMECHANNEL, ENABLED,cache2);
	enableChannel(POSITIONCHANNEL, ENABLED,cache2);
	enableChannel(LIFESPANPPCHANNEL, ENABLED,cache2);
	enableChannel(FINALLIFESPANPPCHANNEL, ENABLED,cache2);
	enableChannel(VELOCITYCHANNEL, ENABLED,cache2);
	enableChannel(RGBPPCHANNEL, ENABLED,cache2);

	// simulation data
	delta=(float)((360.0/getDuration(cache1)));

	// for the entire simulation's length (same values for cache1 and cache2
	for(j=getStartFrame(cache1);j<=getStartFrame(cache1)+getDuration(cache1);j+=getFrameIncrement(cache1))
	{
		color=(float*)malloc(3 * numberOfElements * sizeof(float));
		position=(float*)malloc(3 * numberOfElements * sizeof(float));
		velocity=(float*)malloc(3 * numberOfElements * sizeof(float));
		id=(double*)malloc( numberOfElements * sizeof(double));
		count=numberOfElements;
		birthtime = (double*)calloc(numberOfElements, sizeof(double));
		lifespanPP = (double*)calloc(3*numberOfElements, sizeof(double));
		finalLifespanPP = (double*)calloc(3*numberOfElements, sizeof(double));

		// for each particle in the particle system 
		// compute the position and save the datas
		// position, velocity, color and id for the first particle (x axis)
		id[0]=0;
		position[0]=0;
		position[1]=(float)(r1*sin(3.14*angle1/180.0));
		position[2]=(float)(r1*cos(3.14*angle1/180.0));
		velocity[0]=0;
		velocity[1]=(float)(r1*3.14/180.0*cos(3.14*angle1/180.0));
		velocity[2]=(float)(-r1*3.14/180.0*sin(3.14*angle1/180.0));
		// red color
		color[0]=1.0;
		color[1]=0.0;
		color[2]=0.0;

		// position, velocity and id for the second particle (y axis)
		id[1]=1;
		position[3]=(float)(r2*cos(3.14*angle2/180.0));
		position[4]=0;
		position[5]=(float)(r2*sin(3.14*angle2/180.0));
		velocity[3]=(float)(r2*3.14/180.0*sin(3.14*angle2/180.0));
		velocity[4]=0;
		velocity[5]=(float)(-r2*3.14/180.0*cos(3.14*angle2/180.0));
		// cyan color
		color[3]=0.0;
		color[4]=1.0;
		color[5]=1.0;
		angle1+=delta;
		angle2-=delta;

		// i don't need to compute the birthtime because the particles are already presents at time 0
		// and the "calloc" fill the memory area with zeros. Same appends to lifespanPP and finalLifeSpanPP

		//cache1
		assignChannelValues(COUNTCHANNEL, &count,cache1);
		assignChannelValues(IDCHANNEL,id,cache1);
		assignChannelValues(POSITIONCHANNEL,position,cache1);
		assignChannelValues(VELOCITYCHANNEL,velocity,cache1);
		assignChannelValues(BIRTHTIMECHANNEL,birthtime,cache1);
		assignChannelValues(LIFESPANPPCHANNEL,lifespanPP,cache1);
		assignChannelValues(FINALLIFESPANPPCHANNEL,finalLifespanPP,cache1);
		assignChannelValues(RGBPPCHANNEL,color,cache1);

		//cache2
		assignChannelValues(COUNTCHANNEL, &count,cache2);
		assignChannelValues(IDCHANNEL,id,cache2);
		assignChannelValues(POSITIONCHANNEL,position,cache2);
		assignChannelValues(VELOCITYCHANNEL,velocity,cache2);
		assignChannelValues(BIRTHTIMECHANNEL,birthtime,cache2);
		assignChannelValues(LIFESPANPPCHANNEL,lifespanPP,cache2);
		assignChannelValues(FINALLIFESPANPPCHANNEL,finalLifespanPP,cache2);
		assignChannelValues(RGBPPCHANNEL,color,cache2);

		// chaching simulation
		mayaCache(cache2);

		mayaCache(cache1);
		// free resources
		if(id!=NULL)
			free(id);
		if(position!=NULL)
			free(position);
		if(velocity!=NULL)
			free(velocity);
		if(birthtime!=NULL)
			free(birthtime);
		if(lifespanPP!=NULL)
			free(lifespanPP);
		if(finalLifespanPP!=NULL)
			free(finalLifespanPP);
	}

	// close the maya ncache file and exit
	closeMayaCacheFile(cache1);
	closeMayaCacheFile(cache2);
	printf("Simulation terminated \nPress any key to exit");
	getchar(); 
	
	return 0;
}