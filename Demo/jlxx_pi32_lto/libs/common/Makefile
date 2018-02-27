
LIB_NAME = common


CC_ARG = \
-Os  \
-DNOFLOAT  \


INCLUDES = \
-Iinclude \
-I../include \
-I../../include_lib \
-I../../include_lib/common \
-I../../include_lib/$(LIB_NAME) \
-I../../include_lib/cpu \
-I../../include_lib/cpu/$(RELEASE) \


SRCS_C = \
code/init.c \
$(wildcard code/stdio/*.c) \
$(wildcard code/lib/*.c) \
code/os/sys_timer.c \
code/mem/mem_lw.c \
code/cbuf.c \
# code/uECC.c \
# $(wildcard code/*.c) \
$(wildcard code/mem/*.c) \
code/spin_lock.c \


SRCS_BS = \
$(wildcard *.S) \


-include $(MAKE_RULE)
