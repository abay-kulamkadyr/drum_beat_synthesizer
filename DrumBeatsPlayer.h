/*
Module to play different rock beats in real time.
It will play one the three available Rock Beats (in the order specified below) 
until DrumBeatsPlayer_destroy() function is called.
Order of rock beats: 
1) Standard Rock Beat as specified in section 1 
2) Custom Rock Beat
3) No beats  
Before using the Module, the client code must call DrumBeatsPlayer_init() first, otherwise the result is undefined. 
After DrumBeatsPlayer_init() is called, the standard Rock Beat will be played as default, 
until client calls DrumBeatsPlayer_switchRockBeat()  
After done using the module, the client code must call DrumBeatsPlayer_destroy() to free all resources
*/ 
#ifndef DRUM_BEATS_PLAYER_H
#define DRUM_BEATS_PLAYER_H
enum BEAT_TO_PLAY{
    STANDARD_BEAT,
    CUSTOM_BEAT,
    NO_BEAT,
};
void DrumBeatsPlayer_init(void);
/*
Switches between rock beats in the order specified above
*/
void DrumBeatsPlayer_switchRockBeat(void);
/*
Increases BMP by 5 points
BMP is guaranteed to be in the range [40,300] 
If called on max BMP, BMP does not change 
*/
void DrumBeatsPlayer_increaseBMP(void);
/*
Decreases BPM by 5 points
BMP is guaranteed to be in the range [40,300] 
If called on min BMP, BMP does not change
*/
void DrumBeatsPlayer_decreaseBMP(void);
/*
Increases Volume by 5 points
Volume is guarantedd to be in the range [0,100]
*/
void DrumBeatsPlayer_increaseVolume(void);
/*
Decreases Volume by 5 points
Volume is guarantedd to be in the range [0,100]
*/
void DrumBeatsPlayer_decreaseVolume(void);

//Queues the drum Base sound to playback
void DrumBeatsPlayer_playbackBase(void);
//Queues the drum Snare sound to playback
void DrumBeatsPlayer_playbackSnare(void);
//Queues the drum Hi_hat sound to playback
void DrumBeatsPlayer_playbackHiHat(void);

//Returns the number of the current beat track being played
enum BEAT_TO_PLAY DrumBeatsPlayer_getCurrentBeatMode(void);
//Returns the current volume level
unsigned int DrumBeatsPlayer_getCurrentVolume(void);
//Returns the current tempo (BMP)
unsigned int DrumBeatsPlayer_getCurrentTempo(void);
/*
Set the beat to beatNum
if beatNum set to:
1) Play standard rock beat as specified in section 1
2) Play custom rock beat
3) Play no beats
*/
void DrumBeatsPlayer_setBeatMode(enum BEAT_TO_PLAY beat);
//Frees all resources of the module
void DrumBeatsPlayer_destroy(void);
#endif