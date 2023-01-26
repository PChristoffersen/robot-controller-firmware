#!/usr/bin/env python3

import argparse
import hidutil
from controller import Controller, Output, Input, LUT

vid = 0x0483
pid = 0x5740


def config_arg_to_int(arg: str) -> int:
    if arg=='all':
        return 0xFFFFFFFF
    if arg=='output_configs':
        return (1<<0)
    if arg=='mode_configs':
        return (1<<1)
    if arg=='color_lut':
        return (1<<2)
    if arg=='brightness_lut':
        return (1<<3)

def config_args_to_int(args) -> int:
    arg = 0
    for a in args:
        arg |= config_arg_to_int(a)
    return arg


def main():
    parser = argparse.ArgumentParser()
    cfg_parser = parser.add_subparsers(help="Commands")
    load_parser = cfg_parser.add_parser("load")
    load_parser.add_argument("load", 
        choices=["all", "output_configs", "mode_configs", "color_lut", "brightness_lut"],
        nargs="+"
    )
    store_parser = cfg_parser.add_parser("store")
    store_parser.add_argument("store", 
        choices=["all", "output_configs", "mode_configs", "color_lut", "brightness_lut"],
        nargs="+"
    )
    erase_parser = cfg_parser.add_parser("erase")
    erase_parser.set_defaults(erase=True)

    args = parser.parse_args()

    with hidutil.find_device(vid, pid) as dev:
        controller = Controller(dev)

        if 'load' in args:
            arg = config_args_to_int(args.load)
            print(f"Load: {args.load} {arg:08x}")
            controller.cmd_config_load(arg)
        if 'store' in args:
            arg = config_args_to_int(args.store)
            print(f"Store: {args.store}")
            controller.cmd_config_store(arg)
        if 'erase' in args:
            print(f"Erase: {args.erase}")
            controller.cmd_config_erase()
        


if __name__ == '__main__':
    main()

