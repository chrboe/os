SRCFILES := $(wildcard *.c)
OBJFILES := $(patsubst %.c,%.o,${SRCFILES})

all: ${OBJFILES}

${TARGET_DIR}/%.o: %.c ${TARGET_DIR}
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	${CC} -c ${CFLAGS} -o $@ $<
