CROSS	= arm-none-eabi-
CC	= $(CROSS)gcc
GDB	= $(CROSS)gdb
OBJCOPY	= $(CROSS)objcopy

PROJECT	= rotary

OPENCM3_DIR = lib/libopencm3

SRCS	= $(wildcard src/*.c)
OBJS	= $(SRCS:.c=.o)

COMMON_FLAGS = -Os -g -mthumb -march=armv7-m -mtune=cortex-m3

CFLAGS	 = $(COMMON_FLAGS) -std=c99
CFLAGS	+= -funsigned-char -fomit-frame-pointer
CFLAGS	+= -Wall -Wextra -Werror
CFLAGS	+= -I$(OPENCM3_DIR)/include -DSTM32F1

LDFLAGS	 = $(COMMON_FLAGS) --static -nostartfiles
LDFLAGS	+= -Wl,-Map=$(PROJECT).map
LDFLAGS	+= -T lib/libopencm3/lib/stm32/f1/stm32f103x8.ld

LIBS	 = -L$(OPENCM3_DIR)/lib -lopencm3_stm32f1 -lm

.PHONY: all clean debug flash libopencm3

all: $(PROJECT).bin

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $^ $@

$(PROJECT).elf: libopencm3 $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

libopencm3:
	make -C $(OPENCM3_DIR) TARGETS=stm32/f1

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
	make -C $(OPENCM3_DIR) clean
	rm -f $(OBJS)
	rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).map
