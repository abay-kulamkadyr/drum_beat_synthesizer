#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_
#include <stdbool.h>
void UDP_Init(void);
void UDP_Destroy(void);

struct options
{
    bool option_changeBeat;
    bool option_getCurrentBeat;
    
    bool option_increaseVolume;
    bool option_decreaseVolume;
    bool option_getCurrentVolume;
    
    bool option_increaseBMP;
    bool option_decreaseBMP;
    bool option_getCurrentBMP;

    bool option_switchToStandardBeat;
    bool option_switchToCustomBeat;
    bool option_switchToNoBeat;
    
    bool option_playBase;
    bool option_playSnare;
    bool option_playHiHat;
    bool option_displayUpTime;


};
#endif