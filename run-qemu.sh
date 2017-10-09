#!/usr/bin/env bash

qemu-system-i386 -d cpu_reset -kernel build/kernel -serial file:serial.log -drive file=harddrive.img,format=raw
