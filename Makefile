export BUILD_DIRECTORY := $(abspath build)
export TARGET := ${BUILD_DIRECTORY}/kernel
CODE_DIRECOTRY := $(abspath src)

FILES := $(shell find . -name "*.c")

# colored output
export COLOR_CC := \033[0;33m
export COLOR_LD := \033[0;35m
export COLOR_AS := \033[0;36m
export COLOR_SU := \033[0;32m
export COLOR_SD := \033[0;34m
export COLOR_NO := \033[0m


all: code

${BUILD_DIRECTORY}:
	@echo "Creating build directory..."
	mkdir -p ${BUILD_DIRECTORY}

.PHONY: code
code: ${BUILD_DIRECTORY}
	@${MAKE} -C ${CODE_DIRECOTRY}
	@echo -e "${COLOR_SU}Build successful${COLOR_NO}"

.PHONY: todo
todo:
	-@for file in $(FILES:Makefile=); do fgrep --color=auto -H -e TODO -e FIXME $$file; done; true

.PHONY: clean
clean:
	find . -type f -name "*.o" -delete
	rm -rf ${BUILD_DIRECTORY}
