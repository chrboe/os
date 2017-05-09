SUBDIRS = pc

all: ${SUBDIRS}

.PHONY: ${SUBDIRS}
${SUBDIRS}:
	@echo -e "${COLOR_SD}Building subdirectory $@${COLOR_NO}"
	@${MAKE} -C $@
