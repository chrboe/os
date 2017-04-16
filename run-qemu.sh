#!/usr/bin/env bash

qemu-system-x86_64 -d guest_errors -s -kernel kernel -serial file:serial.log -drive file=harddrive.img,format=raw
