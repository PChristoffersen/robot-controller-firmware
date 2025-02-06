#!/usr/bin/env python3

# BSD 3-Clause License
# 
# Copyright (c) 2025, Peter Christoffersen
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import random
from time import sleep
from lamp_array import LampArray

DEVICE_VID = 0x1209
DEVICE_PID = 0x0001


def get_intensity(value):
    if value <= 100:
        return value
    elif value < 200:
        return 100-(value-100)
    else:
        return 0


def main():
    # Using with statement
    with LampArray(vendor_id=DEVICE_VID, product_id=DEVICE_PID) as lamps:
        device = lamps.device
        print("Manufacturer: %s" % device.manufacturer)
        print("Product: %s" % device.product)
        print("Serial No: %s" % device.serial)
        print("Min update interval: %.3fs" % lamps.min_update_interval)
        print("Number of lamps: %d" % lamps.n_lamps)
        print("")

        n_first = 4
        value = 0

        print("Fade in and out")
        while True:
            value += 4
            lamps[n_first] = (0,0,0,get_intensity(value % 220))
            lamps[n_first+1] = (0,0,0,get_intensity((value+20) % 220))
            lamps[n_first+2] = (0,0,0,get_intensity((value+40) % 220))
            lamps[n_first+3] = (0,0,0,get_intensity((value+60) % 220))
            lamps.show()
            sleep(0.02)





if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Interrupted by user")
