qemu-system-x86_64 -s -S -kernel build/src/kernel/kernel -drive file=harddrive.img,format=raw & sleep 1; gdb build/src/kernel/kernel
