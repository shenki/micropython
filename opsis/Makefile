#OPSIS_DIR=../../opsis-soc/build
#TRIPLE=or1k-elf
#include $(OPSIS_DIR)/software/include/generated/variables.mak
#include $(SOC_DIRECTORY)/software/common.mak

include ../py/mkenv.mk

CROSS = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = or1k-elf-

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I$(BUILD)

#CFLAGS += -fsingle-precision-constant -Wdouble-promotion -mnewlib
CFLAGS += -Wdouble-promotion -nostdlib
CFLAGS += $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

LDFLAGS = -T linker.ld -nostdlib -nodefaultlibs
LIBS =

SRC_C = main.c \
	uart_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(BUILD)/setjmp.o

all: $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ) 
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) crt0-or1k.o -o $@ $^ $(LIBS) libcompiler_rt.a libbase-nofloat.a
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

deploy: $(BUILD)/firmware.bin
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk
