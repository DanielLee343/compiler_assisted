#!/bin/bash

make
sudo rmmod my_char_device.ko
sudo insmod my_char_device.ko
sudo chmod 666 /dev/my_char_device