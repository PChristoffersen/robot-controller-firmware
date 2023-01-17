#!/usr/bin/env python3

import evdev


def main():
    for device in [evdev.InputDevice(path) for path in evdev.list_devices()]:
        print(device)
        #for k,v in device.capabilities().items():
        #    print(f"{k}: {v}")


if __name__ == '__main__':
    main()