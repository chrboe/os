CSRCFILES := $(wildcard *.c)
ASMSRCFILES := $(wildcard *.S)
TARGET_DIR = ${OBJ_DIRECTORY}/kernel/i386/pc
OBJFILES = $(filter %.o,$(CSRCFILES:%.c=${TARGET_DIR}/%.o) $(ASMSRCFILES:%.S=${TARGET_DIR}/%.o))

all: ${OBJFILES}


${TARGET_DIR}/%.o: %.c ${TARGET_DIR}
	@echo -e "${COLOR_CC}CC\t$<${COLOR_NO}"
	${CC} -c ${CFLAGS} -o $@ $<

${TARGET_DIR}/%.o: %.S ${TARGET_DIR}
	@echo -e "${COLOR_AS}AS\t$<${COLOR_NO}"
	@${CC} -c ${CFLAGS} -o $@ $<

${TARGET_DIR}:
	mkdir -p ${TARGET_DIR}
