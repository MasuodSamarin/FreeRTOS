
LIB_NAME = cpu
SUFFIX = _$(RELEASE)


CC_ARG = \


INCLUDES += \
-I$(PLATFORM)/include \
-I../include \
-I../include/$(LIB_NAME) \
-I../../include_lib \
-I../../include_lib/common \
-I../../include_lib/$(LIB_NAME) \
-I../../include_lib/cpu \
-I../../include_lib/cpu/$(RELEASE) \


SRCS_C = \
$(wildcard $(PLATFORM)/*.c) \
$(wildcard code/*.c) \
# code/clock_api.c \
# code/interrupt.c \
# code/power_api.c \


SRCS_BS = \
$(wildcard $(PLATFORM)/*.S) \



-include $(MAKE_RULE)
