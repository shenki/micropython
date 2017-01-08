include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I..
INC += -ITARGET_MCU_NRF51822
INC += -Itargets/cmsis
INC += -I$(BUILD)


CFLAGS += -DNRF51 -DTARGET_NORDIC -DTARGET_M0 -D__MBED__=1 -DMCU_NORDIC_16K
CFLAGS += -DTARGET_NRF51_MICROBIT -DTARGET_MCU_NORDIC_16K
CFLAGS += -DTARGET_MCU_NRF51_16K_S110 -DTARGET_NRF_LFCLK_RC
CFLAGS += -DTARGET_MCU_NORDIC_16K -D__CORTEX_M0 -DARM_MATH_CM0 -DNO_BLE
CFLAGS += -mthumb -mcpu=cortex-m0 -mfloat-abi=soft
CFLAGS += -Wdouble-promotion -Wall -Werror
CFLAGS += -std=gnu11 -fsingle-precision-constant -mabi=aapcs
CFLAGS += $(INC) -ggdb -O2

ifneq ($(DEBUG), 1)
CFLAGS += -DNDEBUG
endif

LDFLAGS = -T NRF51822.ld -Wl,-Map=$@.map -Wl,--cref
LIBS =
LDFLAGS += -Wl,--start-group -lnosys -lm -lc -lgcc -Wl,--end-group  --specs=nano.specs -mabi=aapcs

SRC_C = \
	main.c \
	TARGET_MCU_NRF51822/cmsis_nvic.c \
	TARGET_MCU_NRF51822/system_nrf51.c
#	printf.c \
	string0.c \
	malloc0.c \
	gccollect.c \

SRC_S = \
	TARGET_MCU_NRF51822/TOOLCHAIN_GCC_ARM/startup_NRF51822.S
#	startup_stm32f40xx.s \
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.S=.o))

all: $(BUILD)/firmware.elf

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

include ../py/mkrules.mk
