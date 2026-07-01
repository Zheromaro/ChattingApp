#ifndef CHAT_H
#define CHAT_H

#include <SDL3/SDL.h>

void ChatUpdate(float delta_time);
void ChatInput(SDL_Event* event);
void ChatExit(void);

#endif
