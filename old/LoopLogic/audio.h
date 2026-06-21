#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

bool audioInit();
void audioUpdate();
void audioRelease();
void audioPlayMusic(char soundFile[]);
void audioPauseMusic();
void audioUnpauseMusic();
void audioPlaySFX(char sfxFile[]);

#endif