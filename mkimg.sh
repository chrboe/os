#! /bin/sh

# just create an empty image for now, since the kernel can't handle it anyways :)
dd if=/dev/null of=harddrive.img bs=1 count=0 seek=1M
