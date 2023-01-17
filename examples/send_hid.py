#!/usr/bin/env python3

import hidutil

vid = 0x0483
pid = 0x5740


def main():
    with hidutil.find_device(vid, pid) as dev:
        print(dev.get_feature_report(1))
        neopixel_report = dev.get_feature_report(3)
        print(neopixel_report)
        neopixel_report[1] = 0xFF
        neopixel_report[2] = 0xFF
        neopixel_report[3] = 0xFF
        dev.set_feature_report(3, neopixel_report)


if __name__ == '__main__':
    main()

