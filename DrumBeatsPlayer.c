
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "DrumBeatsPlayer.h"
#include "utils.h"
#include "audioMixer.h"
#define DEFAULT_BPM     120
#define MAX_PATH_SIZE   1024
#define BASEDRUM_FILE   "beatbox-wav-files/base_custom.wav"
#define HIHAT_FILE      "beatbox-wav-files/wavefile.wav"
#define SNARE_FILE      "beatbox-wav-files/snare.wav"
#define HALF_BEAT_TIME_IN_MILLISECONDS(BMP) ( (60000/BMP) / 2 )
//________________________FUNCS TO PLAY ROCK BEATS_______________________________

//Plays back standard rock beat as specified in section 1 
static void DrumBeatsPlayer_StandartRockBeat(void);
/*
Plays back Custom Rock Beat Composition
inspired from: https://musescore.com/user/32386911/scores/5603246
*/
static void DrumBeatsPlayer_CustomRockBeat(void);
//Turns off beat playback
static void DrumBeatsPlayer_TurnOffBeats(void);

//________________________________________________________________________________

//Rock beat playback threading
static void* rockBeatThread(void* arg);
static pthread_t playRockBeatThreadId;
static pthread_mutex_t stopBeats_mutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t stopBeats_condVar=PTHREAD_COND_INITIALIZER;

//wave data structs for each drum sound
static wavedata_t base_sound;
static wavedata_t hihat_sound;
static wavedata_t snare_sound;

//Local variables
static unsigned int BMP; 
static bool terminate_signal;
static bool switchBeat_signal;
static unsigned int halfBeat_time; 
static enum BEAT_TO_PLAY currentPlayingBeat;

void DrumBeatsPlayer_init()
{
    //Paths to wave files
    char baseDrum[MAX_PATH_SIZE];
    char hihatDrum[MAX_PATH_SIZE];
    char snareDrum[MAX_PATH_SIZE];
    snprintf(baseDrum,MAX_PATH_SIZE,BASEDRUM_FILE);
    snprintf(hihatDrum,MAX_PATH_SIZE,HIHAT_FILE);
    snprintf(snareDrum,MAX_PATH_SIZE,SNARE_FILE);
    //Reading wave files into memory
    AudioMixer_readWaveFileIntoMemory(baseDrum, &base_sound);
    AudioMixer_readWaveFileIntoMemory(hihatDrum, &hihat_sound);
    AudioMixer_readWaveFileIntoMemory(snareDrum, &snare_sound);
    AudioMixer_queueSound(&hihat_sound);  
    sleep_ms(1000000000);
    //Setting up static variables
    BMP= DEFAULT_BPM;
    terminate_signal=false;
    switchBeat_signal=false;
    halfBeat_time=HALF_BEAT_TIME_IN_MILLISECONDS(BMP);
    //id of a rock beat
    currentPlayingBeat=STANDARD_BEAT;
    int err=pthread_create(&playRockBeatThreadId, NULL, rockBeatThread, NULL);
    if(err){
        printf("Failed to create Rock Beat playback thread, terminating the programm...\n");
        exit(-1);
    }
}
void DrumBeatsPlayer_increaseBMP()
{
    int checkBMPBoundaries= BMP;
    checkBMPBoundaries+=5;
    if(checkBMPBoundaries<=300 && checkBMPBoundaries>=40){

        BMP=checkBMPBoundaries;
    }
    else{
        
        printf("BMP must be in the range [40,300], current BMP= %d\n", BMP);
    }

}
void DrumBeatsPlayer_decreaseBMP()
{
    int checkBMPBoundaries= BMP;
    checkBMPBoundaries-=5;
    if(checkBMPBoundaries<=300 && checkBMPBoundaries>=40)
    {
        BMP=checkBMPBoundaries;
    }
    else
    {
        printf("BMP must be in the range [40,300], current BMP= %d\n", BMP);
    }

} 
void DrumBeatsPlayer_increaseVolume(void)
{
    int checkVolumeBoundries=AudioMixer_getVolume()+5;
    if(checkVolumeBoundries<=100 && checkVolumeBoundries>=0)
    {
        AudioMixer_setVolume(checkVolumeBoundries);
    }
    else
    {
        printf("Volume must be in the range [0,100], current volume= %d\n",AudioMixer_getVolume());
    }

}
void DrumBeatsPlayer_decreaseVolume(void)
{
    int checkVolumeBoundries=AudioMixer_getVolume()-5;
    if(checkVolumeBoundries<=100 && checkVolumeBoundries>=0)
    {
        AudioMixer_setVolume(checkVolumeBoundries);
    }
    else
    {
        printf("Volume must be in the range [0,100], current volume= %d\n",AudioMixer_getVolume());
    }
}
void DrumBeatsPlayer_TurnOffBeats(void)
{
    pthread_mutex_lock(&stopBeats_mutex);

    while(!switchBeat_signal)
    {
        pthread_cond_wait(&stopBeats_condVar, &stopBeats_mutex);
        
    }
    pthread_mutex_unlock(&stopBeats_mutex);

}
static void* rockBeatThread(void* arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
    while(!terminate_signal)
    {
        switch (currentPlayingBeat)
        {
            case 0:
                DrumBeatsPlayer_StandartRockBeat();
                break;
            
            case 1:
                DrumBeatsPlayer_CustomRockBeat();
                break;

            case 2:
                switchBeat_signal=false;
                DrumBeatsPlayer_TurnOffBeats();
                break;
        }
        switchBeat_signal=false;
    }
    pthread_cond_signal(&stopBeats_condVar);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    sleep_ms(1);
    return NULL;

}
void DrumBeatsPlayer_switchRockBeat(void)
{
    int checkRockBeatNumBoundries = currentPlayingBeat+1;
    if(checkRockBeatNumBoundries>2){
        currentPlayingBeat=STANDARD_BEAT;
    }
    else{
        currentPlayingBeat++;
    }
    printf("Current beat number%d\n", currentPlayingBeat);
    switchBeat_signal=true;
    pthread_cond_signal(&stopBeats_condVar);

}
void DrumBeatsPlayer_setBeatMode(enum BEAT_TO_PLAY beatNum)
{
    switchBeat_signal=true;
    currentPlayingBeat=beatNum;
    pthread_cond_signal(&stopBeats_condVar);
}

void DrumBeatsPlayer_StandartRockBeat()
{
    int loopNumber=0;
    while(!switchBeat_signal)
    {
        //Check current BMP before playing back a beat
        halfBeat_time=HALF_BEAT_TIME_IN_MILLISECONDS(BMP);
        switch (loopNumber)
        {
        case 0:
            AudioMixer_queueSound(&hihat_sound);    
            AudioMixer_queueSound(&base_sound);
            break;
        case 1:
            AudioMixer_queueSound(&hihat_sound);    
            break;
        case 2:
            AudioMixer_queueSound(&hihat_sound);    
            AudioMixer_queueSound(&snare_sound); 
            break;
        case 3:
            AudioMixer_queueSound(&hihat_sound);    
            break;
        case 4:
            AudioMixer_queueSound(&hihat_sound);    
            AudioMixer_queueSound(&base_sound);
            break;
        case 5:
            AudioMixer_queueSound(&hihat_sound);    
            break;
        case 6:
            AudioMixer_queueSound(&hihat_sound);    
            AudioMixer_queueSound(&snare_sound); 
            break;
        case 7:
            AudioMixer_queueSound(&hihat_sound);    
            break;
        }
        sleep_ms(halfBeat_time);
        loopNumber++;
        //repeat the drum sounds once it's 7th iteration
        if(loopNumber==7)
        {
            loopNumber=0;
        }
    }
  
}

void DrumBeatsPlayer_CustomRockBeat()
{
    int loopNumber=0;
 
    while(!switchBeat_signal){
        //Check current BMP before playing back a beat
        halfBeat_time=HALF_BEAT_TIME_IN_MILLISECONDS(BMP);
        switch (loopNumber)
        {
        case 0:
            AudioMixer_queueSound(&hihat_sound);    
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time*2);
            break;
        case 1:
            AudioMixer_queueSound(&snare_sound);    
            sleep_ms(halfBeat_time*2);
            break;
        case 2:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);    
            break;
        case 3:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time); 
            break;
        case 4: 
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 5:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 6:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 7:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        //_____________________________________
        case 8:
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time);
            break;
        case 9:
            AudioMixer_queueSound(&hihat_sound);
            sleep_ms(halfBeat_time);
            break;
        case 10: 
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);
            break;
        case 11: 
            AudioMixer_queueSound(&hihat_sound);
            sleep_ms(halfBeat_time);
            break;
        case 12:
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time);
            break;
        case 13:
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time);
            break;
        case 14:
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);
            break;
        case 15: 
            AudioMixer_queueSound(&hihat_sound);
            sleep_ms(halfBeat_time);
            break;
        //_________________
        case 16:   
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 17:   
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 18:   
            AudioMixer_queueSound(&hihat_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 19:   
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time/2);
            break;
        
        case 20:
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);
            break;
        case 21: 
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);
            break;
        //_____________________________
        case 22: 
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time);
            break;
        case 23: 
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time);
            break;
        case 24: 
            AudioMixer_queueSound(&hihat_sound);
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time);
            break;
        case 25: 
            AudioMixer_queueSound(&hihat_sound);
            sleep_ms(halfBeat_time);
            break;

        case 26: 
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 27:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 28:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 29:
            AudioMixer_queueSound(&snare_sound);
            sleep_ms(halfBeat_time/2);
            break;
        case 30:
            AudioMixer_queueSound(&base_sound);
            sleep_ms(halfBeat_time*4);
            break;
        }
        loopNumber++;

        //repeat the drum sounds once it's 30th iteration
        if(loopNumber==30)
        {
            loopNumber=0;
        }
    }
    
}
unsigned int DrumBeatsPlayer_getCurrentBeatMode(void)
{
    return currentPlayingBeat; 
}
void DrumBeatsPlayer_playbackBase(void)
{
    AudioMixer_queueSound(&base_sound);
}
void DrumBeatsPlayer_playbackSnare(void)
{
    AudioMixer_queueSound(&snare_sound);
}
void DrumBeatsPlayer_playbackHiHat(void)
{
    AudioMixer_queueSound(&hihat_sound);

}
//Returns the current volume level
unsigned int DrumBeatsPlayer_getCurrentVolume(void)
{
    return AudioMixer_getVolume();
}
//Returns the current tempo (BMP)
unsigned int DrumBeatsPlayer_getCurrentTempo(void)
{
    return BMP;
}
void DrumBeatsPlayer_destroy()
{
    terminate_signal=true;
    switchBeat_signal=true;
    pthread_cond_signal(&stopBeats_condVar);
    pthread_cancel(playRockBeatThreadId);
    pthread_join(playRockBeatThreadId, NULL);
    pthread_mutex_destroy(&stopBeats_mutex);
    pthread_cond_destroy(&stopBeats_condVar);
    AudioMixer_freeWaveFileData(&base_sound);
    AudioMixer_freeWaveFileData(&hihat_sound);
    AudioMixer_freeWaveFileData(&snare_sound);
}
