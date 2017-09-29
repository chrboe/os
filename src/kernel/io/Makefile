SRCFILES := $(wildcard *.c)
OBJFILES := $(patsubst %.c,%.o,${SRCFILES})

all: ${OBJFILES}

%.o: %.c
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	@${CC} -c ${CFLAGS} -o $@ $<
