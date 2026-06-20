#include <stddef.h>
#include <stdio.h>
#include <fmod.h>
#include <fmod_errors.h>
#include <fmod_common.h>
#include "LoopLogic/audio.h"

FMOD_SYSTEM *fmodSystem = NULL;
FMOD_CHANNEL *musicChannel = NULL;
FMOD_CHANNEL *sfxChannel = NULL;
FMOD_SOUND *music = NULL;
FMOD_SOUND *sfx = NULL;
FMOD_BOOL state = 0;
FMOD_RESULT result = 1;

bool audioInit() {
    result = FMOD_System_Create(&fmodSystem, FMOD_VERSION);
    if(result != FMOD_OK){
        printf("FMOD could not creat System! FMOD Error: %s\n", FMOD_ErrorString(result));
        return false;
    }

    result = FMOD_System_Init(fmodSystem, 2, FMOD_INIT_NORMAL, NULL);  
    if(result != FMOD_OK){
        printf("FMOD could not be initialized! FMOD Error: %s\n", FMOD_ErrorString(result));
        return false;
    }

    return true;
}
void audioUpdate(){
    FMOD_System_Update(fmodSystem);
}
void audioRelease(){
    
    audioPauseMusic();
    if (music == NULL) FMOD_Sound_Release(music);
    if (sfx == NULL) FMOD_Sound_Release(sfx);

    FMOD_System_Close(fmodSystem);
    FMOD_System_Release(fmodSystem);
}

// for High level code

void audioPlayMusic(char musicFile[]){
    FMOD_Sound_Release(music);
    FMOD_System_CreateSound(fmodSystem, musicFile, FMOD_2D | FMOD_CREATESTREAM | FMOD_DEFAULT | FMOD_LOOP_NORMAL, NULL, &music);
    FMOD_System_PlaySound(fmodSystem, music, NULL, 0, &musicChannel);
}
void audioPauseMusic(){
    FMOD_Channel_GetPaused(musicChannel, &state);
    if (state != 1) // if the music is not paused
        FMOD_Channel_SetPaused(musicChannel, 1); // We pause it
}
void audioUnpauseMusic(){
    FMOD_Channel_GetPaused(musicChannel, &state);
    if (state == 1) // if the music is paused
        FMOD_Channel_SetPaused(musicChannel, 0); // We play it
}
void audioPlaySFX(char sfxFile[]){
    FMOD_System_CreateSound(fmodSystem, sfxFile, FMOD_DEFAULT | FMOD_2D | FMOD_CREATESAMPLE, NULL, &sfx);
    FMOD_System_PlaySound(fmodSystem, sfx, NULL, 0, &sfxChannel);
}