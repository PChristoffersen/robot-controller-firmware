#!/bin/sh

BASEDIR=`dirname $0`

SEND_CMD=$BASEDIR/send_cmd.py

echo ""
echo "Set brightness palette"
$SEND_CMD brightness set  0 0000
$SEND_CMD brightness set  1 FFFF
$SEND_CMD brightness list


echo ""
echo "Set modes"
$SEND_CMD mode set 0 --active_time    0 --passive_time 1000
$SEND_CMD mode set 1 --active_time 1000 --passive_time    0
$SEND_CMD mode set 2 --active_time  500 --passive_time  500
$SEND_CMD mode list


echo ""
echo "Set power button"
$SEND_CMD output set ext_out1 0 --enable false   --active false
$SEND_CMD output set ext_out1 1 --enable false   --active false
$SEND_CMD output set ext_out1 2 --enable false   --active false
$SEND_CMD output set ext_out1 2 --enable button1 --active true    --active_mode 2 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set ext_out1 3 --enable true    --active ext_in1    --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set ext_out2 0 --enable false   --active false
$SEND_CMD output set ext_out2 1 --enable false   --active false
$SEND_CMD output set ext_out2 2 --enable false   --active false
$SEND_CMD output set ext_out2 2 --enable button1 --active true    --active_mode 2 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set ext_out2 3 --enable true    --active ext_in2    --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set led1 0     --enable button1 --active true       --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set led1 1     --enable ext_in1 --active host_ready --active_mode 1 --passive_mode 2 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set led1 2     --enable false   --active true       --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
$SEND_CMD output set led1 3     --enable true    --active true       --active_mode 0 --passive_mode 0 --primary_lut 0 --secondary_lut 0


echo ""
echo "Set activity led"
# TODO
$SEND_CMD output set led2 0     --enable true --active ext_in2       --active_mode 1 --passive_mode 0 --primary_lut 1 --secondary_lut 0
