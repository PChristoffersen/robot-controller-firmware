# Robot Controller

Robot controller for use with [ROS](https://www.ros.org/) 

![Controller](doc/Controller.png)

# Hardware

- [Waveshare 5.5inch AMOLED screen](https://www.waveshare.com/wiki/5.5inch_HDMI_AMOLED)
- Rasbperry Pi 4 or similar
- STM32F103 BluePill micro controller
- 2x JH-D300X-R4 3 Axis joystick
- [Adafruit ANO Rotary Encoder](https://learn.adafruit.com/ano-rotary-encoder)
- 4x [Sparkfun LED Tactile Button](https://www.sparkfun.com/products/10439)
- 4x [Sparkfun LED Tactile Button Breakout](https://www.sparkfun.com/products/10467)
- 4x 12mm Tactile Push button


# Schematic 

See documentation [schematic](doc/Schematic.pdf)

Button breakout board [gerber](doc/Button-breakout-board.zip)
Controller board [gerber](doc/Controller-board.zip)

# BluePill 

## Pinout

|Function|Mode|Pin||Pin|Mode|Function|
|---        |---    |--- |---|--- |---  |---        |
|           |       |    |USB|    |     |           |
|ANO_SW5    |GPIO   |PB12|   |GND |     |           |
|ANO_SW4    |GPIO   |PB13|   |GND |     |           |
|ANO_SW3    |GPIO   |PB14|   |3V3 |     |           |
|ANO_SW2    |GPIO   |PB15|   |NRST|     |           |
|ANO_SW1    |GPIO   |PA8 |   |PB11|GPIO |EXT_OUT2   |
|DEBUG      |UART1  |PA9 |   |PB10|GPIO |EXT_OUT1   |
|DEBUG      |UART1  |PA10|   |PB1 |PWM  |LED4       |
|USB        |USB    |PA11|   |PB0 |PWM  |LED3       |
|USB        |USB    |PA12|   |PA7 |GPIO |EXT_IN2    |
|NEOPIXEL   |PWM    |PA15|   |PA6 |ADC6 |VSENSE     |
|ANO_ENCB   |GPIO   |PB3 |   |PA5 |ADC5 |RIGHT_Z    |
|ANO_ENCA   |GPIO   |PB4 |   |PA4 |ADC4 |RIGHT_Y    |
|EXT_IN1    |GPIO   |PB5 |   |PA3 |ADC3 |RIGHT_X    |
|LED2       |PWM    |PB6 |   |PA2 |ADC2 |LEFT_Z     |
|LED1       |PWM    |PB7 |   |PA1 |ADC1 |LEFT_Y     |
|PCF8574_SCL|SCL1   |PB8 |   |PA0 |ADC0 |LEFT_X     |
|PCF8574_SDA|SDA1   |PB9 |   |PC15|GPIO |PCF8574_INT|
|           |       |5V  |   |PC14|GPIO |AUX        |
|           |       |GND |   |PC13|     |           |
|           |       |3V3 |   |VBAT|     |           |


## PCF8574AN (i2c gpio expander)
|Pin|Function|
|---|---|
|0|RIGHT_X|
|1|RIGHT_Y|
|2|RIGHT_A|
|3|RIGHT_B|
|4|TOP_SW1|
|5|TOP_SW2|
|6|TOP_SW3|
|7|TOP_PWR_SW|


## Timer usage

|Timer|Usage|
|---|---|
|1|Axis ADC|
|2|LED|
|3||
|4||


