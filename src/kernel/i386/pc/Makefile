CSRCFILES := $(wildcard *.c)
ASMSRCFILES := $(wildcard *.S)
OBJFILES = $(filter %.o,$(CSRCFILES:%.c=%.o) $(ASMSRCFILES:%.S=%.o))

all: ${OBJFILES}


%.o: %.c
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	${CC} -c ${CFLAGS} -o $@ $<

%.o: %.S
	@echo -e "${COLOR_AS}AS\t$<${COLOR_NO}"
	i686-elf-as -c -o $@ $<
