#include <stddef.h>
#include "LoopLogic/appSettings.h"

bool *pGameIsRunning = NULL;
bool *pReloadingGame = NULL;

void AppSettingInit(bool *gameIsRunning ,bool *ReloadingGame){
    pGameIsRunning = gameIsRunning;
    pReloadingGame = ReloadingGame;
}

void AppSettingRelease(){
    pGameIsRunning = NULL;
    pReloadingGame = NULL;
}

void CloseGame(){
    *pGameIsRunning = false;
}

void ReloadGame(){
    *pReloadingGame = true;
}