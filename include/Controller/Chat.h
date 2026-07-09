#ifndef CHAT_H
#define CHAT_H

#include <SDL3/SDL.h>

void ChatEnter(void);
void ChatExit(void);
void ChatInput(SDL_Event* event);
void ChatUpdate(float delta_time);

#endif
