#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <stdbool.h>

void AppSettingInit(bool *gameIsRunning ,bool *ReloadingGame);
void AppSettingRelease();
void CloseGame();
void ReloadGame();

#endif