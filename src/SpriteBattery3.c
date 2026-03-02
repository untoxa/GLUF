// same logic as in SpriteBattery1; graphics from the tileset 3

#include "Banks/SetAutoBank.h"

#include "Coroutines.h"
#include "ZGBMain.h"

void BatteryLogic(void * custom_data) BANKED;

SPRITE_COROUTINE(BatteryLogic, NONE)
