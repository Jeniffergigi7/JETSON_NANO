#!/bin/sh

sudo modprobe -r mttcan
sudo modprobe mttcan

sudo ip link set can0 type can bitrate 250000
sudo ip link set up can0
