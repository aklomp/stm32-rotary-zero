PROJECT     = rotary
DEVICE      = stm32f103x8
OPENCM3_DIR = lib/libopencm3

CFLAGS   = -std=c99 -Os -g
CFLAGS  += -funsigned-char -fomit-frame-pointer
CFLAGS  += -Wall -Wextra -Werror

LDFLAGS  = -static -nostartfiles
LDFLAGS += -Wl,-Map=$(PROJECT).map
LIBNAME  = opencm3_stm32f1

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

# Default to silent mode, run 'make V=1' for a verbose build.
ifneq ($(V),1)
Q := @
endif

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: all clean debug flash

all: $(PROJECT).bin

$(LIBDEPS):
	$(MAKE) -C $(OPENCM3_DIR) TARGETS=stm32/f1

flash: $(PROJECT).bin
	openocd \
	  -f interface/stlink-v2.cfg \
	  -c "transport select hla_swd" \
	  -f target/stm32f1x.cfg \
	  -c "init" \
	  -c "reset halt" \
	  -c "stm32f1x unlock 0" \
	  -c "flash write_image erase unlock $(PROJECT).bin 0x08000000" \
	  -c "reset run" \
	  -c "shutdown"

debug: $(PROJECT).elf
	xterm -e 'openocd \
	  -f interface/stlink-v2.cfg \
	  -c "transport select hla_swd" \
	  -f target/stm32f1x.cfg \
	  -c "init" \
	  -c "halt"' &
	$(GDB) --eval-command="target remote localhost:3333" $(PROJECT).elf

clean:
	$(MAKE) -C $(OPENCM3_DIR) clean
	$(RM) $(OBJS) $(LDSCRIPT)
	$(RM) $(PROJECT).bin $(PROJECT).elf $(PROJECT).map

include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk
