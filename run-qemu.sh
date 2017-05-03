#!/usr/bin/env bash

qemu-system-x86_64 -d cpu_reset -s -kernel build/src/kernel/kernel -serial file:serial.log -drive file=harddrive.img,format=raw
