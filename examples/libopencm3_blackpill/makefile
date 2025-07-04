CC = arm-none-eabi-gcc -c
LD = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
OS = arm-none-eabi-size

TARGET = BlackPill_terminal
LIBNAME_OPENCM3 = opencm3_stm32f4


OPT = -Os
DEFS += -DSTM32F4
DIR_OPENCM3 = ./libopencm3
DIR_TEST_COMMANDS = ./my_test_commands
DIR_TERMINAL = ../..
DIR_TERMINAL_lib = $(DIR_TERMINAL)/lib
DIR_TERMINAL_module = $(DIR_TERMINAL)/module
DIR_OBJ = ./.obj
DEBUG = -ggdb3
LDSCRIPT = stm32f4.ld

VPATH = $(DIR_TERMINAL) $(DIR_TERMINAL_lib) $(DIR_TERMINAL_module) $(DIR_TEST_COMMANDS) .

DIR_FLAGS = -I. -I$(DIR_TERMINAL) -I$(DIR_TERMINAL_lib) -I$(DIR_TERMINAL_module) -I$(DIR_TEST_COMMANDS)

ARCH_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

LDFLAGS	+= --static -nostartfiles
LDFLAGS	+= -T$(LDSCRIPT)
LDFLAGS	+= $(ARCH_FLAGS) $(DEBUG)
LDFLAGS	+= -Wl,-Map=$(TARGET).map -Wl,--cref
LDFLAGS	+= -Wl,--gc-sections
ifeq ($(V),99)
LDFLAGS		+= -Wl,--print-gc-sections
endif
LDFLAGS += -u _printf_float
LDFLAGS += -L$(DIR_OPENCM3)/lib

LDLIBS += -l$(LIBNAME_OPENCM3)
LDLIBS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

CFLAGS += $(OPT) $(DEBUG) $(ARCH_FLAGS)
CFLAGS += -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS += -fno-common -ffunction-sections -fdata-sections
CFLAGS += -std=c11 -MMD -MP

DEFS += -I$(DIR_OPENCM3)/include

PREPFLAGS = -MD -Wall -Wundef $(DEFS)

BINFLAGS = -O binary

OBJS = $(DIR_OBJ)/main.o \
		$(DIR_OBJ)/Time.o \
		$(DIR_OBJ)/cli_queue.o \
		$(DIR_OBJ)/cli_string.o \
		$(DIR_OBJ)/cli_input.o \
		$(DIR_OBJ)/cli_log.o \
		$(DIR_OBJ)/cli_time.o \
		$(DIR_OBJ)/terminal.o \
		$(DIR_OBJ)/my_test_commands.o		

all : $(LIBNAME_OPENCM3) $(TARGET).bin

$(LIBNAME_OPENCM3) :
	$(MAKE) -C $(DIR_OPENCM3)

$(TARGET).bin : $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(TARGET).elf
	$(OC) $(TARGET).elf $(BINFLAGS) $@

$(DIR_OBJ)/%.o : %.c
	mkdir -p $(DIR_OBJ)
	$(CC) $(CFLAGS) $(DIR_FLAGS) $(PREPFLAGS) $< -o $@
	
clean :
	rm -rf $(DIR_OBJ) *.bin *.elf *.map *.d
	
-include $(OBJS:.o=.d)
