SRCFILES := $(wildcard *.c)
TARGET_DIR = ${OBJ_DIRECTORY}/kernel/mm
OBJFILES := $(patsubst %.c,${TARGET_DIR}/%.o,${SRCFILES})

all: ${OBJFILES}

${TARGET_DIR}/%.o: %.c ${TARGET_DIR}
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	@${CC} -c ${CFLAGS} -o $@ $<

${TARGET_DIR}:
	mkdir -p ${TARGET_DIR}
