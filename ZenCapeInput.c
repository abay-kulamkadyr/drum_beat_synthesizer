#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "ZenCapeInput.h"
#include "Hardware/joystick.h"
#include "utils.h"
#include "DrumBeatsPlayer.h"
#include "Hardware/accelerometer.h"
#include "audioMixer.h"
#define X_DIRECTION_THREASHOLD  0.4
#define Y_DIRECTION_THREASHOLD  0.4
#define Z_DIRECTION_THREASHOLD  1.5
#define ACCELEROMETER_SENSITIVITY 16384.0
static bool terminate_signal;
static pthread_t ZenCapeControllerThreadID;
static void* ZenCapeControllerThread(void* arg);
void ZenCape_init()
{
    JoyStick_init();
    Accelerometer_init();
    terminate_signal=false;
    int err= pthread_create(&ZenCapeControllerThreadID, NULL, ZenCapeControllerThread,NULL);
    if(err){
        printf("Failed to create Zen Cape controller thread, terminating the programm...\n");
        exit(-1);
    }
}

static void* ZenCapeControllerThread(void* arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
    while(!terminate_signal)
    {
        //Sample every 10 ms
        sleep_ms(10);
        /*
        Sample Accelerometer values and normalize them to g force 
        (using the device's sensitivity which is set 2g by default)
        which easens the detection of accelerations in X-Y-Z directions
        */
        float xDir_value= Accelerometer_getX_Value()/ACCELEROMETER_SENSITIVITY;
        float yDir_value= Accelerometer_getY_Value()/ACCELEROMETER_SENSITIVITY;
        float zDir_value= Accelerometer_getZ_Value()/ACCELEROMETER_SENSITIVITY;
        /*
        Detect accelerations in the X-Y-Z directions based on the threashold values 
        and if acceleration is detected, do appropriate action and sleep to not allow
        other action to trigger (debouncing)
        */
        if(xDir_value>X_DIRECTION_THREASHOLD || xDir_value<-X_DIRECTION_THREASHOLD)
        {
            DrumBeatsPlayer_playbackSnare();
            printf("X value: %f\t", xDir_value);
            printf("Y value: %f\t", yDir_value);
            printf("Z value: %f\n", zDir_value);
            //Wait until back to normal acceleration
            while   ((Accelerometer_getX_Value()/ACCELEROMETER_SENSITIVITY>X_DIRECTION_THREASHOLD) || 
                     ((Accelerometer_getX_Value()/ACCELEROMETER_SENSITIVITY)<-X_DIRECTION_THREASHOLD)){};
            sleep_ms(100);
        }
        if(yDir_value>Y_DIRECTION_THREASHOLD|| yDir_value<-Y_DIRECTION_THREASHOLD)
        {
            DrumBeatsPlayer_playbackHiHat();
            printf("X value: %f\t", xDir_value);
            printf("Y value: %f\t", yDir_value);
            printf("Z value: %f\n", zDir_value);
            //Wait until back to normal acceleration
            while((Accelerometer_getY_Value()/ACCELEROMETER_SENSITIVITY>Y_DIRECTION_THREASHOLD) ||
                  (Accelerometer_getY_Value()/ACCELEROMETER_SENSITIVITY)< -Y_DIRECTION_THREASHOLD){};
            sleep_ms(100);
        }
        if(zDir_value>Z_DIRECTION_THREASHOLD||zDir_value<-Z_DIRECTION_THREASHOLD)
        {
            DrumBeatsPlayer_playbackBase();
            printf("X value: %f\t", xDir_value);
            printf("Y value: %f\t", yDir_value);
            printf("Z value: %f\n", zDir_value);
            //Wait until back to normal acceleration
            while((Accelerometer_getZ_Value()/ACCELEROMETER_SENSITIVITY>Z_DIRECTION_THREASHOLD) ||
                  (Accelerometer_getZ_Value()/ACCELEROMETER_SENSITIVITY)<-Z_DIRECTION_THREASHOLD){};
            
            sleep_ms(100);
        }
        

        /*
        Sample Joystick input values 
        and handle the request with an appropriate action based on the input.
        Sleep 300 ms to allow debouncing
        */
        enum DIRECTION Joystick_currentDirection= JoyStick_getDirection();
        switch (Joystick_currentDirection)
        {
        case DIRECTION_UP:
            DrumBeatsPlayer_increaseVolume();
            sleep_ms(300);
            break;
        
        case DIRECTION_DOWN:
            DrumBeatsPlayer_decreaseVolume();
            sleep_ms(300);
            break;
        case DIRECTION_LEFT:
            DrumBeatsPlayer_decreaseBMP();
            sleep_ms(300);

            break;
        case DIRECTION_RIGHT:
            DrumBeatsPlayer_increaseBMP();
            sleep_ms(300);

            break;
        case DIRECTION_PUSHED:
            DrumBeatsPlayer_switchRockBeat();
            sleep_ms(300);
            break;
        case DIRECTION_NONE:
            break;
        }

    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    sleep_ms(1);
    return NULL;

}
void ZenCape_destroy()
{
    terminate_signal=true;
    pthread_cancel(ZenCapeControllerThreadID);
    pthread_join(ZenCapeControllerThreadID,NULL);
    JoyStick_destroy();
    Accelerometer_destroy();
}