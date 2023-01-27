#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK
import argcomplete
import argparse
import hidutil
from typing import List
from controller import Controller, CommandLoadStoreArg, Output, VirtualInput, Brightness, Color, Colors, ModeConfig, OutputConfigEntry, OutputConfig, Input


def color_completer():
    colors = [ "000000", "FFFFFF"]
    colors.extend(Colors.list())
    return colors


def print_inputs(prefix: str, virt: VirtualInput):
    inputs = [val.name.lower() for val in virt.__class__ if val in virt]
    if inputs:
        print(f"{prefix}: {', '.join(inputs)}")
    else:
        print(f"{prefix}: None")

def print_brightness(lut: List[Brightness]):
    for idx, entry in enumerate(lut):
        print(f"{idx:2d}: {entry.brightness:04x}")

def print_color(lut: List[Color]):
    for idx, color in enumerate(lut):
        print(f"{idx:2d}: {color}")

def print_output_configs(output_configs: List[OutputConfig], offset=0):
    for idx, config in enumerate(output_configs):
        print(f"{offset+idx:2d}: {Output(offset+idx).name}")
        for i, cfg in enumerate(config):
            print(f"    {i} {cfg}")

def print_mode_configs(mode_configs: List[ModeConfig], offset=0):
    for idx, mode in enumerate(mode_configs):
        print(f"{offset+idx:2d}: {mode}")



def main():
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(dest='command')
    subparsers.required = True

    config_parser = subparsers.add_parser("config", help='Config')
    config_subparsers = config_parser.add_subparsers(dest='action')
    config_subparsers.required = True
    config_load_parser = config_subparsers.add_parser('load', help='Load configuration')
    config_load_parser.add_argument("arg", choices=CommandLoadStoreArg.list(), nargs="+")
    config_store_parser = config_subparsers.add_parser('store', help='Store configuration')
    config_store_parser.add_argument("arg", choices=CommandLoadStoreArg.list(), nargs="+")
    config_erase_parser = config_subparsers.add_parser('erase', help='Erase configuration')

    virtual_parser = subparsers.add_parser("virtual", help='Virtual input')
    virtual_subparsers = virtual_parser.add_subparsers(dest='action')
    virtual_subparsers.required = True
    virtual_list_parser = virtual_subparsers.add_parser('list', help='List')
    virtual_set_parser = virtual_subparsers.add_parser('set', help='Set inputs')
    virtual_set_parser.add_argument("inputs", choices=VirtualInput.list(), nargs="+")
    virtual_clear_parser = virtual_subparsers.add_parser('clear', help='Clear inputs')
    virtual_clear_parser.add_argument("inputs", choices=VirtualInput.list(), nargs="+")

    output_parser = subparsers.add_parser("output", help='Output config')
    output_subparsers = output_parser.add_subparsers(dest='action')
    output_subparsers.required = True
    output_list_parser = output_subparsers.add_parser('list', help='List')
    output_set_parser = output_subparsers.add_parser('set', help='Set output')
    output_set_parser.add_argument("output", choices=Output.list())
    output_set_parser.add_argument("index", type=int, choices=range(OutputConfig.ENTRIES_PER_OUTPUT))
    for field,_,bits in OutputConfigEntry.fields_ext():
        if field=='active' or field=='enable':
            output_set_parser.add_argument(f"--{field}", choices=Input.list())
        else:
            output_set_parser.add_argument(f"--{field}", type=int, choices=range((1<<bits)))

    mode_parser = subparsers.add_parser("mode", help='Mode config')
    mode_subparsers = mode_parser.add_subparsers(dest='action')
    mode_subparsers.required = True
    mode_list_parser = mode_subparsers.add_parser('list', help='List')
    mode_set_parser = mode_subparsers.add_parser('set', help='Set mode')
    mode_set_parser.add_argument("index", type=int)
    for field in ModeConfig.fields():
        mode_set_parser.add_argument(f"--{field}", type=int)

    color_parser = subparsers.add_parser("color", help='Color LUT')
    color_subparsers = color_parser.add_subparsers(dest='action')
    color_subparsers.required = True
    color_list_parser = color_subparsers.add_parser('list', help='List')
    color_set_parser = color_subparsers.add_parser('set', help='Set color')
    color_set_parser.add_argument("offset", type=int)
    color_set_parser.add_argument("values", nargs="+", help=f"Example: {color_completer()}").completer=color_completer

    brightness_parser = subparsers.add_parser("brightness", help='Brightness LUT')
    brightness_subparsers = brightness_parser.add_subparsers(dest='action')
    brightness_subparsers.required = True
    brightness_list_parser = brightness_subparsers.add_parser('list', help='List')
    brightness_set_parser = brightness_subparsers.add_parser('set', help='Set brightness')
    brightness_set_parser.add_argument("offset", type=int)
    brightness_set_parser.add_argument("values", nargs="+")

    argcomplete.autocomplete(parser)
    args = parser.parse_args()


    with hidutil.find_device(Controller.VID, Controller.PID) as dev:
        controller = Controller(dev)

        if args.command == 'config':
            if args.action == 'load':
                arg = CommandLoadStoreArg.from_list(args.arg)
                print(f"Config load: {args.arg}")
                controller.cmd_config_load(arg)
            elif args.action == 'store':
                arg = CommandLoadStoreArg.from_list(args.arg)
                print(f"Config store: {args.arg}")
                controller.cmd_config_store(arg)
            elif args.action == 'erase':
                print(f"Config erase")
                controller.cmd_config_erase()

        elif args.command == 'virtual':
            if args.action == 'list':
                print_inputs("Inputs", controller.virtual_input)
            if args.action == 'set':
                vals = VirtualInput.from_list(args.inputs)
                print_inputs("Set", vals)
                controller.virtual_input |= vals
                controller.update_virtual_input()
                print_inputs("Result", controller.virtual_input)
            if args.action == 'clear':
                vals = VirtualInput.from_list(args.inputs)
                print_inputs("Clear", vals)
                controller.virtual_input &= ~vals
                controller.update_virtual_input()
                print_inputs("Result", controller.virtual_input)

        elif args.command == 'output':
            if args.action == 'list':
                print_output_configs(controller.output_configs)
            elif args.action == 'set':
                output = Output.from_string(args.output)
                config = controller.output_configs[output.value]
                slot = config[args.index]
                for field,_,_ in OutputConfigEntry.fields_ext():
                    val = getattr(args, field)
                    if val is None:
                        continue
                    if field=='enable' or field=='active':
                        val = Input.from_string(val).value
                    setattr(slot, field, val)
                controller.update_output_configs()
                print_output_configs([config], offset=output.value)


        elif args.command == 'mode':
            if args.action == 'list':
                print_mode_configs(controller.mode_configs)
            elif args.action == 'set':
                config = controller.mode_configs[args.index]
                for field in ModeConfig.fields():
                    val = getattr(args, field)
                    if val is not None:
                        setattr(config, field, val)
                controller.update_mode_configs()
                print_mode_configs(controller.mode_configs)


        elif args.command == 'color':
            if args.action == 'list':
                print_color(controller.color_lut)
            elif args.action == 'set':
                lut = controller.color_lut
                for i,b in enumerate(args.values):
                    lut[args.offset+i].set_from_string(b)
                controller.update_color_lut()
                print_color(controller.color_lut)

        elif args.command == 'brightness':
            if args.action == 'list':
                print_brightness(controller.brightness_lut)
            elif args.action == 'set':
                lut = controller.brightness_lut
                for i,b in enumerate(args.values):
                    lut[args.offset+i].set_from_string(b)
                controller.update_brightness_lut()
                print_brightness(controller.brightness_lut)


if __name__ == '__main__':
    main()

