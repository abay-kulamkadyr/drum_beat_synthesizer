
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h> 
#include <netdb.h>
#include <string.h>			
#include <unistd.h>
#include <math.h>
#include <limits.h>

#include "UDP_Listener.h"
#include "DrumBeatsPlayer.h"
#include "utils.h"

#define PORT 12345
#define MAX_COMMAND_SIZE 100
#define NUMBER_OF_COMMANDS 15
#define MAX_UDP_SIZE 1500 

static void* UDPListener_thread(void* attr);
static pthread_t UDPListener_threadID; 
static int socketDescriptor;
static bool requestToShutdown;

static struct options parseOptions (char* request);
static struct sockaddr_in sinRemote;
static unsigned int sin_len = sizeof(sinRemote);
static void processRequests(struct options* opts, char * message);



static char listOf_commands[NUMBER_OF_COMMANDS][MAX_COMMAND_SIZE]={
    "changeBeat\n",             //0
    "getCurrentBeat\n",         //1

    "increaseVolume\n",         //2
    "decreaseVolume\n",         //3
    "Volumeget\n",              //4
    "increaseBMP\n",            //5
    "decreaseBMP\n",            //6
    "BMPget\n",                 //7

    "switchToStandardBeat\n",   //8
    "switchToCustomBeat\n",     //9
    "switchToNoBeat\n",         //10

    "BasePlay\n",               //11
    "SnarePlay\n",              //12
    "HiHatPlay\n",              //13
    "displayUpTime\n"           //14
};

static void initOptions(struct options* getOption )
{
    getOption->option_changeBeat=false;
    getOption->option_getCurrentBeat=false;
    getOption->option_increaseVolume=false;
    getOption->option_decreaseVolume=false;
    getOption->option_getCurrentVolume=false;
    getOption->option_increaseBMP=false;
    getOption->option_decreaseBMP=false;

    getOption->option_getCurrentBMP=false;
    getOption->option_switchToStandardBeat=false;
    getOption->option_switchToCustomBeat=false;
    getOption->option_switchToNoBeat=false;

    getOption->option_playBase=false;
    getOption->option_playSnare=false;
    getOption->option_playHiHat=false;
    getOption->option_displayUpTime=false;
}
static struct options parseOptions (char* request)
{
    //creating a struct of options to infer which option has been requested
    struct options passedOptions;
    //initiallizing all options to false initially 
    initOptions(&passedOptions);
    int options=0;
 
    /*
    Comparing the passed string to find the requested option
    */

    for(int i=0;i<NUMBER_OF_COMMANDS;i++)
    {
        if((strcmp(listOf_commands[i], request))==0)
        {
            options=i;
            break;
        }
    }
   
    char* messageTx;
    switch (options)
    {
        case 0:
            passedOptions.option_changeBeat=true;
            break;
        case 1:
            passedOptions.option_getCurrentBeat=true;
            break;
        case 2:
            passedOptions.option_increaseVolume=true; 
            break;
        case 3:
            passedOptions.option_decreaseVolume=true;
            break;
        case 4:
            passedOptions.option_getCurrentVolume=true;
            break;
        case 5:
            passedOptions.option_increaseBMP=true;
            break;
        case 6:
            passedOptions.option_decreaseBMP=true;
            break;
        case 7:
            passedOptions.option_getCurrentBMP=true;
            break;
        case 8:
            passedOptions.option_switchToStandardBeat=true;
            break;
        case 9:
            passedOptions.option_switchToCustomBeat=true;

            break;
        case 10:
            passedOptions.option_switchToNoBeat=true;
            break;
        case 11:
            passedOptions.option_playBase=true;
            break;
        case 12:
            passedOptions.option_playSnare=true;
            break;
        case 13:
            passedOptions.option_playHiHat=true;
            break;
        case 14:
            passedOptions.option_displayUpTime=true;
            break;
        default:
            messageTx= "Sorry the specified options are not currently supported\n";
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
            messageTx, strlen(messageTx),
            0,
            (struct sockaddr *) &sinRemote, sin_len); 
            break;
    }
    
    return passedOptions;
}

void UDP_Init(void)
{
    requestToShutdown=false;
    struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));	

    pthread_create(&UDPListener_threadID, NULL, UDPListener_thread, NULL);

}
void UDP_Destroy(void)
{
    requestToShutdown=true;
    pthread_cancel(UDPListener_threadID);
    pthread_join(UDPListener_threadID, NULL);
    close(socketDescriptor);

}
static void processRequests(struct options* opts, char* messageRx)
{

    if(opts->option_changeBeat)
    {
        DrumBeatsPlayer_switchRockBeat();
    }
    else if(opts-> option_getCurrentBeat)
    {
        char messageTx[MAX_UDP_SIZE];
        enum BEAT_TO_PLAY currentBeat=DrumBeatsPlayer_getCurrentBeatMode();
        sprintf(messageTx, "%d\n", currentBeat);
        
        sendto( socketDescriptor,
			messageTx, strlen(messageTx),
			0,
			(struct sockaddr *) &sinRemote, sin_len);
    }
    else if(opts->option_increaseVolume)
    {
        DrumBeatsPlayer_increaseVolume();
    }
    else if(opts->option_decreaseVolume)
    {
        DrumBeatsPlayer_decreaseVolume();
    }
    else if(opts->option_getCurrentVolume)
    {
        char messageTx[MAX_UDP_SIZE];
        unsigned int volume= DrumBeatsPlayer_getCurrentVolume();
        sprintf(messageTx, "%d\n", volume);
        
        sendto( socketDescriptor,
			messageTx, strlen(messageTx),
			0,
			(struct sockaddr *) &sinRemote, sin_len);
    }
    else if(opts->option_increaseBMP)
    {
        DrumBeatsPlayer_increaseBMP();
    }
    else if(opts->option_decreaseBMP)
    {
        DrumBeatsPlayer_decreaseBMP();
    }
    else if(opts->option_getCurrentBMP)
    {
        char messageTx[MAX_UDP_SIZE];
        unsigned int currentBMP= DrumBeatsPlayer_getCurrentTempo();
        sprintf(messageTx, "%d\n", currentBMP);
        
        sendto( socketDescriptor,
			messageTx, strlen(messageTx),
			0,
			(struct sockaddr *) &sinRemote, sin_len);
    }
    else if(opts->option_switchToStandardBeat)
    {
        DrumBeatsPlayer_setBeatMode(STANDARD_BEAT);
    }
    else if(opts->option_switchToCustomBeat)
    {
        DrumBeatsPlayer_setBeatMode(CUSTOM_BEAT);
    }
    else if(opts->option_switchToNoBeat)
    {
        DrumBeatsPlayer_setBeatMode(NO_BEAT);
    }
    else if(opts->option_playBase)
    {
        DrumBeatsPlayer_playbackBase();
    }
    else if(opts->option_playSnare)
    {
        DrumBeatsPlayer_playbackSnare();
    }
    else if(opts->option_playHiHat)
    {
        DrumBeatsPlayer_playbackHiHat();
    }
    else if(opts->option_displayUpTime)
    {
        char messageTx[MAX_UDP_SIZE];
        FILE* upTime_fptr;
        char buff[28];
        long uptime=0;
        if((upTime_fptr=fopen("/proc/uptime","r"))==NULL)
            perror("Coulnd't open proc/uptime\n");
        fgets(buff, 12, upTime_fptr);
        fclose(upTime_fptr);
        uptime= strtol(buff,NULL,10);
        int hours=uptime/3600;
        uptime-=(hours*3600);
        int minutes= uptime/60;
        uptime-=(minutes*60);
        int seconds= uptime;
        sprintf(messageTx, "%d:%d:%d(H:M:S)\n", hours,minutes,seconds);
        
        sendto( socketDescriptor,
			messageTx, strlen(messageTx),
			0,
			(struct sockaddr *) &sinRemote, sin_len);

    }

}
static void* UDPListener_thread(void* attr)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
  
 
    while(!requestToShutdown)
    {
        // Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		
		char messageRx[MAX_COMMAND_SIZE];
        // Pass buffer size - 1 for max # bytes so room for the null (string data)
        int bytesRx = recvfrom(socketDescriptor,
			messageRx, MAX_COMMAND_SIZE - 1, 0,
			(struct sockaddr *) &sinRemote, &sin_len);
        messageRx[bytesRx] = 0;
    
        struct options opts= parseOptions(messageRx);
        processRequests(&opts, messageRx);

    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    sleep_ms(1);
    return NULL;
}