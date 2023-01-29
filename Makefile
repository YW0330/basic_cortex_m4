# 編譯執行的檔案名(要包含 main 函式)
FILENAME ?= main
# COM port
PORT = 4

# include 的路徑
INCLUDEDIR = ./include 
## Device settings
MACH = cortex-m4
# Link script
LS = STM32F303ZETX_FLASH.ld
# OpenOCD configure
OPENOCDCFG = st_nucleo_f3.cfg
# release/debug mode
MODE ?= release


# ----- 以下沒有特殊需求不需要進行修改 -----
# 存放測試主程式的資料夾名稱(要包含 main 函式)
TESTDIR = ./test
# 自定義的函式資料夾名稱
SRCDIR = ./src ./startup
# Build dir
BUILD_DIR = ./build
# Dist dir
DIST_DIR = ./dist

## Cross compiler
CROSS = arm-none-eabi-
CC = $(CROSS)gcc
SZ = $(COMPILER)size

## Generate include options
INCLUDEPATH = $(foreach include, $(INCLUDEDIR), -I$(include))

## Options common to compile, link and assembly rules
COMMON = -mcpu=$(MACH) -mthumb
COMMON += -mfloat-abi=soft

## Compile flags
CFLAGS = $(COMMON)
CFLAGS +=  -Wall
CFLAGS += $(INCLUDEPATH)
ifeq ($(MODE),debug)
CFLAGS += -O0 -g
else
CFLAGS += -Os
endif

## Options common to linker
LDCOMMON = -T$(LS) -Wl,-Map=$(BUILD_DIR)/$(FILENAME).map
## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += $(LDCOMMON)
LDFLAGS += -specs=nano.specs

## Generate object files from source
# The source files from c language
FIND_SRCS = $(wildcard $(dir)/*.c)
# The source files from assembly 
FIND_SRCS += $(wildcard $(dir)/*.s)
SRCS = $(foreach dir, $(SRCDIR), $(FIND_SRCS))
SRCOBJS = $(addprefix $(BUILD_DIR)/,$(filter %.o,$(patsubst %.c,%.o,$(notdir $(SRCS))) \
    $(patsubst %.s,%.o,$(notdir $(SRCS)))))
OBJS = $(SRCOBJS)

## Generate object files from target file
TARGETSRC = $(TESTDIR)/$(FILENAME).c
TARGETOBJ = $(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(notdir $(TARGETSRC))))
TARGETELF = $(addprefix $(DIST_DIR)/, $(patsubst %.c, %.elf, $(notdir $(TARGETSRC))))
OBJS += $(TARGETOBJ)

vpath %.c $(SRCDIR)
vpath %.s $(SRCDIR)
vpath %.c $(TESTDIR)

default: clean makedir $(TARGETELF)

makedir:
	@mkdir $(BUILD_DIR)
	@mkdir $(DIST_DIR)

$(DIST_DIR)/%.elf: $(OBJS)
	@$(CC) $(LDFLAGS) $^ -o $@
	@$(SZ) $@

$(BUILD_DIR)/%.o: %.c 
	@$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/%.o: %.s
	@$(CC) -c $(CFLAGS) $< -o $@


.SECONDARY: $(OBJS)
disassembly: $(TARGETELF)
	@$(CROSS)objdump.exe -d $< > $(DIST_DIR)/$(FILENAME).s
load:
	openocd -f board/$(OPENOCDCFG)
upload:
	openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c " program $(TARGETELF) verify exit "
terminal_debug:
	putty -telnet -P 50002 localhost
terminal_openocd:
	putty -telnet -P 4444 localhost
terminal_serial:
	putty -serial COM$(PORT) -sercfg 38400,8,1,n,N
	
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DIST_DIR)


