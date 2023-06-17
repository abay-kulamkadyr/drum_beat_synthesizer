/*
The module continiously samples inputs from the Zen-Cape (on a separate thread)
and decides which action to take based on these inputs
Actions triggered by the Joystick inputs:
    If input is UP:     Increases the volume by 5 points 
    If input is DOWN:   Decreases the volume by 5 points
    If input is LEFT:   Decreases the tempo by 5 BPM
    If input is RIGHT:  Increases the tempo by 5 BPM
    If input is PUSHED: Cicles through the beats
Actions triggered by the Accelerometer: 
    If acceleration in the X-axis is detected: playback the Snare sound
    If acceleration in the Y-axis is detected: playback the Hi-hat sound
    If acceleration in the Z-axis is detected: playback the Base sound
*/
#ifndef ZENCAPE_INPUT_H
#define ZENCAPE_INPUT_H
//Initializes all the hardware devices
void ZenCape_init(void);
//Destroy all the hardware devices
void ZenCape_destroy(void);
#endif