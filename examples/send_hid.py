#!/usr/bin/env python3

from time import sleep
import hidutil
from controller import Controller, Output, Input, LUT

vid = 0x0483
pid = 0x5740


def main():
    with hidutil.find_device(vid, pid) as dev:
        controller = Controller(dev)

        controller.output_configs[Output.LED1.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED1.value][0].active = Input.BUTTON1.value
        controller.output_configs[Output.LED1.value][0].active_mode  = 2
        controller.output_configs[Output.LED1.value][0].passive_mode = 3
        controller.output_configs[Output.LED1.value][0].primary_lut  = 2
        controller.output_configs[Output.LED1.value][0].secondary_lut  = 3

        controller.output_configs[Output.LED2.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED2.value][0].active = Input.BUTTON2.value
        controller.output_configs[Output.LED2.value][0].active_mode  = 2
        controller.output_configs[Output.LED2.value][0].passive_mode = 3
        controller.output_configs[Output.LED2.value][0].primary_lut  = 2
        controller.output_configs[Output.LED2.value][0].secondary_lut  = 3

        controller.output_configs[Output.LED3.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED3.value][0].active = Input.BUTTON3.value
        controller.output_configs[Output.LED3.value][0].active_mode  = 2
        controller.output_configs[Output.LED3.value][0].passive_mode = 3
        controller.output_configs[Output.LED3.value][0].primary_lut  = 2
        controller.output_configs[Output.LED3.value][0].secondary_lut  = 3

        controller.output_configs[Output.LED4.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED4.value][0].active = Input.BUTTON4.value
        controller.output_configs[Output.LED4.value][0].active_mode  = 2
        controller.output_configs[Output.LED4.value][0].passive_mode = 3
        controller.output_configs[Output.LED4.value][0].primary_lut  = 2
        controller.output_configs[Output.LED4.value][0].secondary_lut  = 3

        controller.output_configs[Output.NEOPIXEL1.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.NEOPIXEL1.value][0].active = Input.TRUE.value
        controller.output_configs[Output.NEOPIXEL1.value][0].active_mode  = 4
        controller.output_configs[Output.NEOPIXEL1.value][0].passive_mode = 3
        controller.output_configs[Output.NEOPIXEL1.value][0].primary_lut  = 10
        controller.output_configs[Output.NEOPIXEL1.value][0].secondary_lut  = 11

        #controller.output_configs[Output.NEOPIXEL2.value][0].enable = Input.TRUE.value
        #controller.output_configs[Output.NEOPIXEL2.value][0].active = Input.TRUE.value
        #controller.output_configs[Output.NEOPIXEL2.value][0].active_mode  = 4
        #controller.output_configs[Output.NEOPIXEL2.value][0].passive_mode = 3
        #controller.output_configs[Output.NEOPIXEL2.value][0].primary_lut  = 11
        #controller.output_configs[Output.NEOPIXEL2.value][0].secondary_lut  = 10

        #controller.output_configs[Output.NEOPIXEL3.value][0].enable = Input.TRUE.value
        #controller.output_configs[Output.NEOPIXEL3.value][0].active = Input.TRUE.value
        #controller.output_configs[Output.NEOPIXEL3.value][0].active_mode  = 4
        #controller.output_configs[Output.NEOPIXEL3.value][0].passive_mode = 3
        #controller.output_configs[Output.NEOPIXEL3.value][0].primary_lut  = 12
        #controller.output_configs[Output.NEOPIXEL3.value][0].secondary_lut  = 10
        controller.update_output_configs()

        controller.mode_configs[2].active_time = 1000
        controller.mode_configs[2].passive_time = 0
        controller.mode_configs[3].active_time = 0
        controller.mode_configs[3].passive_time = 1000
        
        controller.mode_configs[4].rise_time = 1000
        controller.mode_configs[4].active_time = 500
        controller.mode_configs[4].fall_time = 1000
        controller.mode_configs[4].passive_time = 500
        controller.update_mode_configs()

        controller.brightness_lut[2].brightness = 0xFFFF
        controller.brightness_lut[3].brightness = 0x0000
        controller.update_brightness_lut()

        controller.color_lut[10].red   = 0x20
        controller.color_lut[10].green = 0x00
        controller.color_lut[10].blue  = 0x00
        controller.color_lut[11].red   = 0x00
        controller.color_lut[11].green = 0x00
        controller.color_lut[11].blue  = 0x20
        controller.color_lut[12].red   = 0x00
        controller.color_lut[12].green = 0x20
        controller.color_lut[12].blue  = 0x00
        controller.update_color_lut()


if __name__ == '__main__':
    main()

