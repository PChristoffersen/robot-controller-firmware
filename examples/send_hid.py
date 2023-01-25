#!/usr/bin/env python3

from time import sleep
import hidutil
from controller import Controller, Output, Input

vid = 0x0483
pid = 0x5740


def main():
    with hidutil.find_device(vid, pid) as dev:
        controller = Controller(dev)

        controller.output_configs[Output.LED1.value][0].enable_inv = False
        controller.output_configs[Output.LED1.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED1.value][0].active_inv = False
        controller.output_configs[Output.LED1.value][0].active = Input.BUTTON1.value
        controller.output_configs[Output.LED1.value][0].active_mode  = 2
        controller.output_configs[Output.LED1.value][0].passive_mode = 3
        controller.output_configs[Output.LED1.value][0].primary_lut   = 2
        controller.output_configs[Output.LED1.value][0].passive_lut  = 3

        controller.output_configs[Output.LED2.value][0].enable_inv = False
        controller.output_configs[Output.LED2.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED2.value][0].active_inv = False
        controller.output_configs[Output.LED2.value][0].active = Input.BUTTON2.value
        controller.output_configs[Output.LED2.value][0].active_mode  = 2
        controller.output_configs[Output.LED2.value][0].passive_mode = 3
        controller.output_configs[Output.LED2.value][0].primary_lut   = 2
        controller.output_configs[Output.LED2.value][0].passive_lut  = 3

        controller.output_configs[Output.LED3.value][0].enable_inv = False
        controller.output_configs[Output.LED3.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED3.value][0].active_inv = False
        controller.output_configs[Output.LED3.value][0].active = Input.BUTTON3.value
        controller.output_configs[Output.LED3.value][0].active_mode  = 2
        controller.output_configs[Output.LED3.value][0].passive_mode = 3
        controller.output_configs[Output.LED3.value][0].primary_lut   = 2
        controller.output_configs[Output.LED3.value][0].passive_lut  = 3

        controller.output_configs[Output.LED4.value][0].enable_inv = False
        controller.output_configs[Output.LED4.value][0].enable = Input.TRUE.value
        controller.output_configs[Output.LED4.value][0].active_inv = False
        controller.output_configs[Output.LED4.value][0].active = Input.BUTTON4.value
        controller.output_configs[Output.LED4.value][0].active_mode  = 2
        controller.output_configs[Output.LED4.value][0].passive_mode = 3
        controller.output_configs[Output.LED4.value][0].primary_lut   = 2
        controller.output_configs[Output.LED4.value][0].passive_lut  = 3
        controller.update_output_configs()

        controller.brightness_lut[2].brightness = 0xFFFF
        controller.brightness_lut[3].brightness = 0x0100
        controller.update_brightness_lut()

        controller.mode_configs[2].active_time = 1000
        controller.mode_configs[2].passive_time = 0
        controller.mode_configs[3].active_time = 0
        controller.mode_configs[3].passive_time = 1000
        controller.update_mode_configs()



if __name__ == '__main__':
    main()

