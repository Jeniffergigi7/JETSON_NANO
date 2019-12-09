#!/bin/sh

sudo modprobe can
sudo modprobe can-raw
sudo modprobe mttcan

sudo ip link set can0 type can bitrate 500000 sample-point 0.80
sudo ip link set up can0
