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

# Add application code
SRCS_C = \
cpu/$(RELEASE)/debug_init.c \
cpu/$(RELEASE)/sys_init.c \
cpu/$(RELEASE)/uart_hw.c \
cpu/$(RELEASE)/timer_hw.c \

# Setup paths to source code	
export SOURCE_PATH 	= freertos/Source
export PORT_PATH 	= freertos/Source/portable/pi32_lto/$(RELEASE)
export DEMO_PATH 	= freertos/Demo/Common/Minimal
export LOCAL_PATH 	= freertos/Demo/${RELEASE}

INCLUDES += \
-Ifreertos/Source/include \
-Ifreertos/Source/portable/pi32_lto/$(RELEASE) \
-Ifreertos/Demo/Common/include \
-Ifreertos/Demo/${RELEASE} \

# Add core code for FreeRTOS base
SRCS_C += \
$(SOURCE_PATH)/tasks.c \
$(SOURCE_PATH)/list.c \
$(SOURCE_PATH)/queue.c \
$(SOURCE_PATH)/portable/MemMang/heap_1.c \
$(DEMO_PATH)/flash.c \
$(DEMO_PATH)/integer.c \
$(DEMO_PATH)/comtest.c \
$(DEMO_PATH)/PollQ.c \
$(PORT_PATH)/port.c \
$(LOCAL_PATH)/serial/serial.c \
$(LOCAL_PATH)/ParTest/ParTest.c \
$(LOCAL_PATH)/main.c
# $(PORT_PATH)/portISR.c \


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

