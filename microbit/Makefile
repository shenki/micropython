include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I..

# mbed HAL headers
INC += -Imbed/hal
INC += -Imbed/common
INC += -Imbed/api

# CMSIS headers
INC += -Imbed/targets/cmsis

# CMSIS headers for NRF51822
INC += -Imbed/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822

# mbed HAL implementation for NRF51822
INC += -Imbed/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822
INC += -Imbed/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/TARGET_NRF51_MICROBIT

# For qstr outputs.
INC += -I$(BUILD)

CFLAGS += -DNRF51 -DTARGET_NORDIC -DTARGET_M0 -D__MBED__=1 -DMCU_NORDIC_16K
CFLAGS += -DTARGET_NRF51_MICROBIT -DTARGET_MCU_NORDIC_16K
CFLAGS += -DTARGET_MCU_NRF51_16K_S110 -DTARGET_NRF_LFCLK_RC
CFLAGS += -DTARGET_MCU_NORDIC_16K -D__CORTEX_M0 -DARM_MATH_CM0 -DNO_BLE
CFLAGS += -D__thumb2__
CFLAGS += -mthumb -mcpu=cortex-m0 -mfloat-abi=soft
CFLAGS += -Wdouble-promotion -Wall -Werror
CFLAGS += -std=gnu11 -fsingle-precision-constant -mabi=aapcs
CFLAGS += $(INC) -ggdb -Og

ifneq ($(DEBUG), 1)
CFLAGS += -DNDEBUG
endif

LDFLAGS = -L mbed/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM/TARGET_MCU_NRF51_16K_S110 -T NRF51822.ld -Wl,-Map=$@.map -Wl,--cref
LIBS =
LDFLAGS += -Wl,--start-group -lnosys -lm -lc -lgcc -Wl,--end-group  --specs=nano.specs -mabi=aapcs

SRC_LIB =$(addprefix lib/,\
    mp-readline/readline.c \
	utils/pyexec.c \
	utils/interrupt_char.c \
	)

SRC_C = \
	main.c \
    modutime.c

SRC_CMSIS = \
	mbed/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/cmsis_nvic.c \
	mbed/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/system_nrf51.c

SRC_HAL = \
    mbed/common/ticker_api.c \
    mbed/common/us_ticker_api.c \
	mbed/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/pinmap.c \
	mbed/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/serial_api.c \
	mbed/targets/hal/TARGET_NORDIC/TARGET_MCU_NRF51822/us_ticker.c

SRC_S = \
	mbed/targets/cmsis/TARGET_NORDIC/TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM/startup_NRF51822.S

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CMSIS:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_LIB)

.PHONY: pyodc gdb all

all: $(BUILD)/firmware.hex

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O ihex $^ $@
	srec_cat $@ -intel -o $@ -intel --line-length=44

gdb: $(BUILD)/firmware.elf
	$(Q)$(GDB) -q $(BUILD)/firmware.elf -ex "target remote localhost:3333" -ex "load"

pyocd:
	pyocd-gdbserver -d debug

include ../py/mkrules.mk
