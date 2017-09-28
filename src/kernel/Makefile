SRCFILES := $(wildcard *.c)
TARGET_DIR = ${OBJ_DIRECTORY}/kernel
OBJFILES := $(patsubst %.c,${TARGET_DIR}/%.o,${SRCFILES})
LD_SCRIPT = $(abspath i386/pc/layout.ld)
LD_OBJS := $(shell find ${TARGET_DIR} -type f -name "*.o")
SUBDIRS := mm io i386

# include directories
INCLUDES = $(abspath .)

# C compiler
# TODO: might wanna change this for other ABIs
export CC = i686-elf-gcc
export LD = i686-elf-gcc

# C flags
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wconversion -Wstrict-prototypes

export CFLAGS = -I${INCLUDES} -fno-stack-protector -ffreestanding -g #${WARNINGS}

# link flags
export LDFLAGS = -nostdlib -lgcc

all: ${SUBDIRS} ${OBJFILES}
	@echo -e "${COLOR_LD}LD\t${TARGET}${COLOR_NO}"
	@${LD} ${LDFLAGS} ${LD_OBJS} -T ${LD_SCRIPT} -o ${TARGET}

${TARGET_DIR}:
	mkdir -p ${TARGET_DIR}

${TARGET_DIR}/%.o: %.c ${TARGET_DIR}
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	@${CC} -c ${CFLAGS} -o $@ $<

.PHONY: ${SUBDIRS}
${SUBDIRS}:
	@${MAKE} -C $@
