
# Schematic 

See documentation [schematic](doc/Schematic.pdf)


# Pinout

|Function|Mode|P||P|Mode|Function|
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


# PCF8574AN (i2c gpio expander)
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


# Unassigned

PI_ACT1
PI_ACT2
PI_ACT3
PI_ACT4

PI_PWR_SIG
VBATT
BATT_EN



# Timer usage

|Timer|Usage|
|---|---|
|1|Axis ADC|
|2|LED|
|3||
|4||


# DMA1:
|Channel|Function|
|---|---|
|1|ADC1, TIM2_CH3, TIM4_CH1|
|2|USART3_TX, TIM1_CH1, TIM2_UP,  TIM3_CH3, SPI1_RX|
|3|USART3_RX, TIM1_CH2, TIM3_CH4, TIM3_UP,  SPI1_TX|
|4|USART1_TX, TIM1_CH4, TIM1_TRIG, TIM1_COM, TIM4_CH2, SPI2/I2S2_RX, |I2C2_TX
|5|USART1_RX, TIM1_UP, TIM2_CH1, TIM4_CH3, SPI2/I2S2_TX, I2C2_RX|
|6|USART2_RX, TIM1_CH3, TIM3_CH1, TIM3_TRIG, I2C1_TX|
|7|USART2_TX, TIM2_CH2, TIM2_CH4, TIM4_UP, I2C1_RX|


## Timer 1
|Timer|00|11|DMA|
|---|---|---|---|
|TIM1_CH1|PA8 |PE9 |2
|TIM1_CH2|PA9 |PE11|3
|TIM1_CH3|PA10|PE13|6
|TIM1_CH4|PA11|PE14|4


## Timer 2
|Timer|00|01|10|11|DMA|
|---|---|---|---|---|---|
|TIM2_CH1_ETR|PA0|PA15|PA0|PA15|5
|TIM2_CH2    |PA1|PB3 |PA1|PB3 |7
|TIM2_CH3    |PA2|PB10|   |    |1
|TIM2_CH4    |PA3|PB11|   |    |7


## Timer 3
|Timer|00|01|11|DMA|
|---|---|---|---|---|
|TIM3_CH1|PA6|PB4||6|
|TIM3_CH2|PA7|PB5|||
|TIM3_CH3|PB0|PC8||2|
|TIM3_CH4|PB1|PC9||3|


## Timer 4
|Timer|0|1|DMA|
|---|---|---|---|
|TIM4_CH1|PB6|PD12|1|
|TIM4_CH2|PB7|PD13|4|
|TIM4_CH3|PB8|PD14|5|
|TIM4_CH4|PB9|PD15||



# HID Features

## Input
|Bit|Function|
| ---|---|
| 0|True|
| 1|USB Led1|
| 2|USB Led2|
| 3|USB Led3|
| 4|USB Led4|
| 5|USB Led5|
| 6|EXT_IN1|
| 7|EXT_IN2|
| 8|BUTTON1|
| 9|BUTTON2|
|10|BUTTON3|
|11|BUTTON4|
|12|BUTTON5|
|13|BUTTON6|
|14|BUTTON7|
|15|BUTTON8|
|16|ANO_SW1|
|17|ANO_SW2|
|18|ANO_SW3|
|19|ANO_SW4|
|20|ANO_SW5|
||Logic1|
||Logic2|
||Logic3|
||Logic4|


## Config 4 settings pr LED/Neopixel output: 
|1      |2  |5      |1      |2  |5      |4      |4       |
|---    |---|---    |---    |---|---    |---    |---     |
|EInv   |Res|Enable |AInv   |Res|Active |On Mode|Off mode|


## Settings (16): 
* Invert enable
* Invert active
* Behaviour
    * Static
    * Blink on/off time
    * Fade on/off, up,down time
* Active LUT
* Passive LUT
* Fallthrough on passive

## RGB LUT (16): 
* 24 bit RGB Colors

## LED LUT (16): 
* 16bit Brightness level


# External output map
Invert?
|4 EXT_OUT1|4 EXT_OUT2|
|---    |---    |
|Input  |Input  |
