# OS
This is my (currently non-functioning) pet project, a small little operating system.

## Current Development Status
Some things are working, most aren't. This is still in the very early stages. Currently I'm trying hard to get paging set up and working properly (I just can't quite seem to wrap my head around the concept). When I get that done, it's basically capable of printing `a` and `b` to the screen in rapid succession.

Also note that this has been a private project for quite some time now, and I've only now decided to publish the source code. As a result the code contains some ugly hacks and needs to be cleaned up in general.

## Building
First of all, you need to build a cross compiler (32-bit gcc, e.g. i686-elf-gcc). If you're running Arch Linux, you can use the [i686-elf-gcc](https://aur.archlinux.org/packages/i686-elf-gcc/) AUR package.
You'll also need to install qemu and change the command in `run-qemu.sh` to your appropriate qemu command.

Then,
```
$ make
```

If for some magical reason it actually compiles, just run `run-qemu.sh` and a qemu window SHOULD pop up.

## TODO
(In order from most to least important)

* Paging!
* Probably make multitasking work properly
* ext2 support
* ELF parsing
* ARM support
