#!/usr/bin/env bash

qemu-system-x86_64 -d cpu_reset -kernel build/kernel -serial file:serial.log -drive file=harddrive.img,format=raw
