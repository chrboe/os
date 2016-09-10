qemu-system-x86_64 -s -S -kernel kernel -drive file=harddrive.img,format=raw & sleep 1; gdb -x gdbargs
