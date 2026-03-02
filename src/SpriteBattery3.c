#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

void BatteryLogic(void * custom_data) BANKED;

SPRITE_COROUTINE(BatteryLogic, NONE)
