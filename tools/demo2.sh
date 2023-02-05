#!/bin/sh

BASEDIR=`dirname $0`

SEND_CMD=$BASEDIR/send_cmd.py


echo ""
echo "Set color palette"
$SEND_CMD color set 0 black
$SEND_CMD color set 1 red
$SEND_CMD color list

echo ""
echo "Set modes"
$SEND_CMD mode set 0 --active_time    0 --passive_time 1000
$SEND_CMD mode set 1 --active_time 1000 --passive_time    0
$SEND_CMD mode list


echo ""
echo "Set outputs"
$SEND_CMD output set neopixel1 0 --active usb_led_message  --enable true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel2 0 --active usb_led_generic  --enable true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel3 0 --active usb_led_charging --enable true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel4 0 --active usb_led_standby  --enable true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
