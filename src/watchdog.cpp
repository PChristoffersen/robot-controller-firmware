#include "watchdog.h"

#include <Arduino.h>
#include <libmaple/iwdg.h>


void watchdog_init()
{
    iwdg_init(IWDG_PRE_16, 0xFFF);
}


void watchdog_tick()
{
    iwdg_feed();
}
