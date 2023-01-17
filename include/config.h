#pragma once

#include <Arduino.h>

#define ENABLE_USB 1
#define ENABLE_WATCHDOG 0
#define ENABLE_DEBUG 1

static constexpr uint8 ANO_ENCA_PIN      { PB3  };
static constexpr uint8 ANO_ENCB_PIN      { PB4  };
static constexpr uint8 ANO_SW1_PIN       { PA8  };
static constexpr uint8 ANO_SW2_PIN       { PB15 };
static constexpr uint8 ANO_SW3_PIN       { PB14 };
static constexpr uint8 ANO_SW4_PIN       { PB13 };
static constexpr uint8 ANO_SW5_PIN       { PB12 };

static constexpr uint8 LED1_PIN          { PB7 };
static constexpr uint8 LED2_PIN          { PB6 };
static constexpr uint8 LED3_PIN          { PB0 };
static constexpr uint8 LED4_PIN          { PB1 };

static constexpr uint8 AXIS_LEFT_X_PIN   { PA0 };
static constexpr uint8 AXIS_LEFT_Y_PIN   { PA1 };
static constexpr uint8 AXIS_LEFT_Z_PIN   { PA2 };
static constexpr uint8 AXIS_RIGHT_X_PIN  { PA3 };
static constexpr uint8 AXIS_RIGHT_Y_PIN  { PA4 };
static constexpr uint8 AXIS_RIGHT_Z_PIN  { PA5 };
static constexpr uint8 VSENSE_PIN        { PA6 };

static constexpr uint8 EXT_IN1           { PB5  };
static constexpr uint8 EXT_IN2           { PA7  };
static constexpr uint8 EXT_OUT1          { PB10 };
static constexpr uint8 EXT_OUT2          { PB11 };

static constexpr uint8 AUX_PIN           { PC14 };
static constexpr uint8 PCF8574_INT_PIN   { PC15 };

static constexpr size_t LED_COUNT { 4 };
static constexpr size_t NEOPIXEL_COUNT { 4 };
static constexpr size_t NEOPIXEL_BYTES_PER_PIXEL { 3 };
