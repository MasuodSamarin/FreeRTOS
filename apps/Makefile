CC_ARG = \
-DNOFLOAT  \
-DHAVE_MALLOC  \

CC_ARG += \
-D$(SoC)  \


INCLUDES = \
-Iinclude \
-Iinclude/$(RELEASE) \
-Icpu/$(RELEASE) \
-Icommon \
-I../include_lib \
-I../include_lib/common \
-I../include_lib/cpu \
-I../include_lib/cpu/$(RELEASE) \
-I../include_lib/btstack \
-Ifreertos/include \
-Ifreertos/portable/pi32_lto/$(RELEASE) \
-Ifreertos/Common/include \

# Setup paths to source code	
# export SOURCE_PATH 	= ../../../Source
# export PORT_PATH 	= ../../../Source/portable/pi32_lto/jlxx/$(RELEASE)
# export DEMO_PATH 	= ../../Common/Minimal

# Add core code for FreeRTOS base

# Add application code
SRCS_C = \
cpu/$(RELEASE)/debug_init.c \
cpu/$(RELEASE)/sys_init.c \
cpu/$(RELEASE)/uart_hw.c \
main.c

# SRCS_C = \
# $(SOURCE_PATH)/tasks.c \
# $(SOURCE_PATH)/list.c \
# $(SOURCE_PATH)/queue.c \
# $(SOURCE_PATH)/portable/MemMang/heap_1.c \
# $(DEMO_PATH)/flash.c \
# $(DEMO_PATH)/integer.c \
# $(DEMO_PATH)/comtest.c \
# $(DEMO_PATH)/PollQ.c \
# $(PORT_PATH)/portISR.c \
# $(PORT_PATH)/port.c \


ifeq ($(LE_MODE), CONTROLLER_MODE)
# SRCS_C += \
bt/iut/h4_uart.c

# SRCS_C += \
bt/dut/h4_4010A.c

SRCS_C += \
bt/dongle/usb_dongle.c

else
# SRCS_C += \
# bt/bt_task.c \
# bt/examples/le_streamer.c \
# bt/examples/le_streamer_client.c \
# bt/examples/user.c \
bt/examples/hogp_server.c \

endif

SRCS_BS = \
cpu/$(RELEASE)/startup.S \


LIBS = \
--start-group \
../include_lib/common_lib.a \
../include_lib/cpu_lib_$(RELEASE).a  \
--end-group \


-include $(MAKE_RULE)


