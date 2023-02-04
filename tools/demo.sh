#!/bin/sh

BASEDIR=`dirname $0`

SEND_CMD=$BASEDIR/send_cmd.py


echo ""
echo "Set color palette"
$SEND_CMD color set 0 black
$SEND_CMD color set 1 202020 # White
$SEND_CMD color set 2 200000 # Red
$SEND_CMD color set 3 002000 # Green
$SEND_CMD color set 4 000020 # Blue
$SEND_CMD color set 5 202000 # Yellow
$SEND_CMD color set 6 002020
$SEND_CMD color list

echo ""
echo "Set brightness palette"
$SEND_CMD brightness set  0 0000
$SEND_CMD brightness set  1 FFFF
$SEND_CMD brightness set  2 7FFF
$SEND_CMD brightness set  3 1000
$SEND_CMD brightness list


echo ""
echo "Set modes"
$SEND_CMD mode set 0 --active_time    0 --passive_time 1000
$SEND_CMD mode set 1 --active_time 1000 --passive_time    0
$SEND_CMD mode set 2 --active_time  500 --passive_time  500
$SEND_CMD mode set 3 --active_time  500 --passive_time  500 --rise_time  200 --fall_time  200
$SEND_CMD mode set 4 --active_time  500 --passive_time  500 --rise_time 1000 --fall_time 1000
$SEND_CMD mode set 5 --active_time  200 --passive_time  200
$SEND_CMD mode list


echo ""
echo "Set outputs"
$SEND_CMD output set led1 0 --enable true    --active true    --active_mode 4 --passive_mode 0 --primary_lut 2 --secondary_lut 0

$SEND_CMD output set led2 0 --enable true    --active button2 --active_mode 1 --passive_mode 0 --primary_lut 2 --secondary_lut 0

$SEND_CMD output set led3 0 --enable button3 --active button3 --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set led3 1 --enable true    --active true    --active_mode 3 --passive_mode 0 --primary_lut 3 --secondary_lut 0

$SEND_CMD output set led4 0 --enable true    --active button4 --active_mode 1 --passive_mode 0 --primary_lut 2 --secondary_lut 3


$SEND_CMD output set neopixel1 0 --enable ano_sw1  --active true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel2 0 --enable ano_sw1  --active true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel3 0 --enable ano_sw1  --active true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set neopixel4 0 --enable ano_sw1  --active true --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0

#$SEND_CMD output set neopixel1 1 --enable true     --active true --active_mode 1 --passive_mode 0 --primary_lut 2 --secondary_lut 0
#$SEND_CMD output set neopixel2 1 --enable true     --active true --active_mode 1 --passive_mode 0 --primary_lut 3 --secondary_lut 0
#$SEND_CMD output set neopixel3 1 --enable true     --active true --active_mode 1 --passive_mode 0 --primary_lut 4 --secondary_lut 0
#$SEND_CMD output set neopixel4 1 --enable true     --active true --active_mode 1 --passive_mode 0 --primary_lut 5 --secondary_lut 0

#$SEND_CMD output set neopixel1 1 --enable true    --active button1 --active_mode 1 --passive_mode 0 --primary_lut 5 --secondary_lut 6
#$SEND_CMD output set neopixel2 1 --enable true    --active button5 --active_mode 5 --passive_mode 0 --primary_lut 2 --secondary_lut 6
#$SEND_CMD output set neopixel3 1 --enable true    --active true    --active_mode 3 --passive_mode 0 --primary_lut 3 --secondary_lut 0
#$SEND_CMD output set neopixel4 1 --enable button5 --active true    --active_mode 3 --passive_mode 0 --primary_lut 4 --secondary_lut 5
#$SEND_CMD output set neopixel4 2 --enable true    --active true    --active_mode 3 --passive_mode 0 --primary_lut 2 --secondary_lut 3
