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

