
#include <alsa/asoundlib.h>

#include "audioMixer.h"
#include "DrumBeatsPlayer.h"
#include "ZenCapeInput.h"
#include "UDP_Listener.h"

int main(void)
{
    AudioMixer_init();
    DrumBeatsPlayer_init();
    ZenCape_init();
    UDP_Init();    
    //Make the program run for 5 mins
    sleep(300);
    
    UDP_Destroy();
    ZenCape_destroy();    
    DrumBeatsPlayer_destroy();
    AudioMixer_cleanup();

	printf("Done!\n");
	return 0;
}

