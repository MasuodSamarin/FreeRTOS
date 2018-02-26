#/*
# * FreeRTOS Kernel V10.0.1
# * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * http://www.FreeRTOS.org
# * http://aws.amazon.com/freertos
# *
# * 1 tab == 4 spaces!
# */

# --------------basic setting-----------------------
# ifeq ($(LANG),)
ifeq ($(shell uname),Linux)
export HOST_OS = linux
export SLASH=/
else
export HOST_OS = windows
export SLASH=\\
endif

# 配置库文件是否带后缀(None/...)
export SUFFIX=

# 默认注释编译信息(None/@)
export V=@

#配置下载目标SoC(br16/br17/br20)
export SoC=br20

#*************************************************************************
#						Linux
#
#*************************************************************************
ifeq ($(HOST_OS), linux) 
# --------------tools path-----------------------
# export JL_TOOLS=/opt/pi32/test
export JL_TOOLS=/opt/pi32/rel_40

export CC = $(JL_TOOLS)/bin/pi32-clang
export LD = $(JL_TOOLS)/bin/lto-wrapper
export AR = $(JL_TOOLS)/bin/lto-ar
 
export OBJCOPY=$(JL_TOOLS)/bin/pi32-objcopy
export OBJDUMP=$(JL_TOOLS)/bin/pi32-objdump

export VER=$(JL_TOOLS)/bin/replacetime
 
endif #($(HOST_OS), linux)

#*************************************************************************
#
#						Windows
#
#*************************************************************************
ifeq ($(HOST_OS), windows) 
# --------------tools path-----------------------
export JL_TOOLS=C:/JL/pi32

export CC = $(JL_TOOLS)/bin/clang.exe
export LD = $(JL_TOOLS)/bin/lto-wrapper.exe
export AR = $(JL_TOOLS)/bin/lto-ar.exe

export OBJCOPY=$(JL_TOOLS)/bin/llvm-objcopy.exe
export OBJDUMP=$(JL_TOOLS)/bin/llvm-objdump.exe

export VER=$(JL_TOOLS)/bin/replacetime.exe

endif #($(HOST_OS), linux)
#*************************************************************************
#
#						Common
#
#*************************************************************************
# --------------include path-----------------------
export SYS_INCLUDES = \
-I$(JL_TOOLS)/include  \
-I$(JL_TOOLS)/include/libc  \

# --------------complier args-----------------------
export AR_ARGS = \
-r -s 

export LD_ARGS = \
-L$(JL_TOOLS)/include/libc \
--plugin-opt=-pi32-memreg-opt \
--plugin-opt=-pi32-mem-offset-adj-opt \
--plugin-opt=-pi32-const-spill \
--plugin-opt=-pi32-enable-jz \
--plugin-opt=-pi32-tailcall-opt \
--plugin-opt=-inline-threshold=5 \
--plugin-opt=-pi32-enable-itblock=1 \
--plugin-opt=save-temps \
-o \
 

export CC_ARGS = \
-target pi32 \
-integrated-as \
-fno-builtin \
-c -g \
-Oz \
-flto \
-w \

# -Wall  \
-Wno-unused-function \
-Wno-unused-const-variable \
-Wno-unused-variable \


export CC_ARGS += \
-fms-extensions \
-DSUPPORT_MS_EXTENSIONS \


export SYS_LIBS = \
$(JL_TOOLS)/lib/libc.a  \
$(JL_TOOLS)/lib/libcompiler-rt.a \



# --------------Basic Setting-----------------------
#  
#配置下载CPU架构(bfin/pi32/pi32_lto)
export ARCH = pi32_lto

#配置下载目标平台(br16/br17/br20)
export RELEASE=br20

#配置项目文件指向(br16/br17/br20)
export PLATFORM=br20

#配置是否FPGA 验证平台(FPGA/CHIP)
export TYPE=CHIP

#配置下载匹配(br17_sdk/br20_sdk)
export NICKNAME=br20_sdk

# 配置BLE 蓝牙模式(CONTROLLER_MODE/FULL_MODE)
export LE_MODE=CONTROLLER_MODE

#配置下载介质(otp/flash)
export CODE_SPACE=otp
#配置OTP调试(y/n)
export DEBUG=y
#  
# --------------Project path Setting-----------------------
#  
export POST_BUILD_DIR= post_build$(SLASH)$(RELEASE)$(SLASH)$(CODE_SPACE)

export DIR_OUTPUT= $(POST_BUILD_DIR)$(SLASH)debug

export LIB_DIR = output

export DEPENDS = $(abspath .)/depend.bat

# - output file
export ELF= sdk
OUTPUT_ELF= $(addprefix $(DIR_OUTPUT)$(SLASH), $(ELF))
export OUTPUT_ELF:= $(addsuffix .exe, $(OUTPUT_ELF))

BUILD_LD=sdk.ld
BUILD_MAP=sdk.map

export LINKER = \
-T$(addprefix $(POST_BUILD_DIR)$(SLASH), $(BUILD_LD).out) \
-M=$(addprefix $(POST_BUILD_DIR)$(SLASH), $(BUILD_MAP)) 

export LIBS_PATH=libs

export INCLUDE_LIB_PATH=..$(SLASH)..$(SLASH)include_lib

#default TARGET
export TARGET=apps

export MAKEDIR = $(shell find $(LIBS_PATH) -maxdepth 1 -type d)

ifeq ($(HOST_OS), windows) 
CPP=C:\JL\dv10\\bin\dv10-elf-cpp.exe
endif

ifeq ($(HOST_OS), linux) 
CPP=cpp
endif
#  
# --------------Architecture & Complier Setting-----------------------
#  
-include tools$(SLASH)compiler$(SLASH)Makefile.$(ARCH)

LINKER_SCRIPT_INPUT = \
apps$(SLASH)post_build$(SLASH)$(RELEASE)$(SLASH)$(CODE_SPACE)$(SLASH)$(BUILD_LD)

ENTRY_LIST=post_build$(SLASH)$(RELEASE)$(SLASH)libs_stubs.ld

pre_make:
	@echo "Making... Soc : br20 $(CODE_SPACE)"
ifeq ($(CODE_SPACE), flash)
	$(V) $(CPP) -P -DENTRY_LIST=$(ENTRY_LIST) $(LINKER_SCRIPT_INPUT) $(LINKER_SCRIPT_INPUT).out
endif
ifeq ($(CODE_SPACE), otp)
ifeq ($(DEBUG), y)
	$(V) $(CPP) -P -DCODE_START_ADDR=0x1000000 -DCODE_LENGTH=16M -DENTRY_LIST=$(ENTRY_LIST) $(LINKER_SCRIPT_INPUT) $(LINKER_SCRIPT_INPUT).out
else 
	$(V) $(CPP) -P -DCODE_START_ADDR=0x1ff0200 -DCODE_LENGTH=64K-512 -DENTRY_LIST=$(ENTRY_LIST) $(LINKER_SCRIPT_INPUT) $(LINKER_SCRIPT_INPUT).out
endif
endif

# Setup paths to source code
SOURCE_PATH = ../../Source
PORT_PATH = ../../Source/portable/pi32_lto/br20
DEMO_PATH = ../Common/Minimal


CC_ARG = \
-DNOFLOAT  \
-DHAVE_MALLOC  \

INCLUDES = \
-I../../Source/include \
-I../../Source/portable/pi32_lto/br20 \
-I../Common/include \

#
# Source files that can be built to THUMB mode.
#
SRCS_C = \
$(SOURCE_PATH)/tasks.c \
$(SOURCE_PATH)/list.c \
$(SOURCE_PATH)/queue.c \
$(SOURCE_PATH)/portable/MemMang/heap_1.c \
$(DEMO_PATH)/flash.c \
$(DEMO_PATH)/integer.c \
$(DEMO_PATH)/comtest.c \
$(DEMO_PATH)/PollQ.c
$(PORT_PATH)/port.c \
main.c \

SRCS_BS = \
startup.S \

#
# Define all object files.
#
# - src file
SRCS_C:= $(abspath $(SRCS_C))
SRCS_BS:= $(abspath $(SRCS_BS))
SRCS_LS:= $(abspath $(SRCS_LS))

OBJS_C = $(SRCS_C:.c=.o)
OBJS_BS = $(SRCS_BS:.S=.o)
OBJS_LS = $(SRCS_LS:.s=.o)

DEPS_C = $(SRCS_C:.c=.d)
DEPS_BS = $(SRCS_BS:.S=.d)
DEPS_LS = $(SRCS_LS:.s=.d)

ARCHIVE =  $(addsuffix _lib$(SUFFIX).a, $(LIB_NAME))

#*************************************************************************
#
#						Windows
#
#*************************************************************************
ifeq ($(HOST_OS), windows) 

.PHONY: all archive clean dry_run
 
all: $(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	$(V) $(LD) $(LD_ARGS) $(OUTPUT_ELF) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) $(LIBS) $(SYS_LIBS) $(LINKER) 
	@$(DIR_OUTPUT)\download.bat $(ELF) $(OBJDUMP) $(OBJCOPY) $(BANKZIP)

archive:$(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	@if exist $(LIB_DIR) (rd /s/q $(LIB_DIR))
	@mkdir $(LIB_DIR)
	@if exist $(INCLUDE_LIB_PATH)\$(ARCHIVE) del $(INCLUDE_LIB_PATH)\$(ARCHIVE)
	$(V) $(AR) $(AR_ARGS) $(LIB_DIR)\$(ARCHIVE) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) 
	@copy $(LIB_DIR)\*.a $(INCLUDE_LIB_PATH)  /Y
	@echo Make $(ARCHIVE) success!

clean:
	@for /r %%i in (*.o) do del %%i
	@for /r %%i in (*.d) do del %%i
	@for /r %%i in (*.d.1) do del %%i
	@if exist $(OUTPUT_ELF) del $(OUTPUT_ELF)
	@if exist $(INCLUDE_LIB_PATH)\$(ARCHIVE) del $(INCLUDE_LIB_PATH)\$(ARCHIVE)
 
 
$(OBJS_LS):%.o:%.s
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	@echo + ss $<
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) -D__ASSEMBLY__ $(CC_ARGS) -c $< -o $@
 
$(OBJS_BS):%.o:%.S
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) -D__ASSEMBLY__ $(CC_ARGS) $(CC_ARG) -c $< -o $@
 
$(OBJS_C):%.o:%.c
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	@echo + CC $<
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) $(CC_ARGS) $(CC_ARG) -c $< -o $@

 
$(DEPS_C):%.d:%.c
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< > $@.1
	$(V) $(DEPENDS) $@ $(dir $@) $(subst /,\,$@)

$(DEPS_BS):%.d:%.S
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< > $@.1
	$(V) $(DEPENDS) $@ $(dir $@) $(subst /,\,$@)

$(DEPS_LS):%.d:%.s
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< > $@.1
	$(V) $(DEPENDS) $@ $(dir $@) $(subst /,\,$@)
 
 

endif

#*************************************************************************
#
#						Linux
#
#*************************************************************************
ifeq ($(HOST_OS), linux) 

.PHONY: all archive clean dry_run
 
all: premake $(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	$(V) $(LD) $(LD_ARGS) $(OUTPUT_ELF) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) $(LIBS) $(SYS_LIBS) $(LINKER)
	@cd $(DIR_OUTPUT) && $(SHELL) download.sh $(ELF)
	@echo Make $(OUTPUT_ELF) success!

archive:$(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	@if [ ! -d $(LIB_DIR) ]; then mkdir $(LIB_DIR); fi
	@if [ -f $(LIB_DIR)/*.a ]; then rm -f $(LIB_DIR)/*.a; fi
	@if [ -f $(INCLUDE_LIB_PATH)/$(ARCHIVE) ]; then rm -f $(INCLUDE_LIB_PATH)/$(ARCHIVE); fi
	$(V) $(AR) $(AR_ARGS) $(LIB_DIR)/$(ARCHIVE) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) 
	@cp $(LIB_DIR)/*.a $(INCLUDE_LIB_PATH)
	@echo Make $(ARCHIVE) success!

clean:
	@ -rm -rf $(INCLUDE_LIB_PATH)/$(ARCHIVE)
	@ -rm -rf $(OUTPUT_ELF)
	@ -find -name "*.[od]" | xargs rm -f 
	@ echo Clean ok...

dry_run:$(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	@echo Congratulation!

 
$(OBJS_LS):%.o:%.s
	@if [ -f $@ ]; then rm $@; fi
	@echo + ss $<
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) -D__ASSEMBLY__ $(CC_ARGS) -c $< -o $@
 
$(OBJS_BS):%.o:%.S
	@if [ -f $@ ]; then rm $@; fi
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) -D__ASSEMBLY__ $(CC_ARGS) $(CC_ARG) -c $< -o $@
 
$(OBJS_C):%.o:%.c
	@if [ -f $@ ]; then rm $@; fi
	@echo + CC $<
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) $(CC_ARGS) $(CC_ARG) -c $< -o $@

 # 
 #
$(DEPS_C):%.d:%.c
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< | sed 's,\($(notdir $*)\)\.o[:]*,$(dir $@)\1.o $@ : ,g' > $@

$(DEPS_BS):%.d:%.S
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< | sed 's,\($(notdir $*)\)\.o[:]*,$(dir $@)\1.o $@ : ,g' > $@

$(DEPS_LS):%.d:%.s
	$(V) $(CC) -MM $(SYS_INCLUDES) $(INCLUDES) $< | sed 's,\($(notdir $*)\)\.o[:]*,$(dir $@)\1.o $@ : ,g' > $@

endif
 

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS_C)
endif
	








	


