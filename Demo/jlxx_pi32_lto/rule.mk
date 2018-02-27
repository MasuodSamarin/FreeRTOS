 

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
	$(V) $(LD) $(LD_ARGS) $(OUTPUT_EXES) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) $(LIBS) $(SYS_LIBS) $(LINKER) --output-version-info $(DIR_OUTPUT)/ver.bin
	@$(DIR_OUTPUT)\download.bat $(EXES) $(OBJDUMP) $(OBJCOPY) $(BANKZIP)

debug:
	@echo "--LS"
	@echo $(OBJS_LS) 
	@echo "--BS"
	@echo $(OBJS_BS) 
	@echo "--C"
	@echo $(OBJS_C) 

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
	@if exist $(OUTPUT_EXES) del $(OUTPUT_EXES)
	@if exist $(INCLUDE_LIB_PATH)\$(ARCHIVE) del $(INCLUDE_LIB_PATH)\$(ARCHIVE)
 
$(OBJS_LS):%.o:%.s
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	@echo + ss $<
	$(V) $(CC)  $(SYS_INCLUDES) $(INCLUDES) -D__ASSEMBLY__ $(CC_ARGS) -c $< -o $@
 
$(OBJS_BS):%.o:%.S
	@if exist $(subst /,\,$@) del $(subst /,\,$@)
	@echo + SS $<
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
 
all: $(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	$(V) $(LD) $(LD_ARGS) $(OUTPUT_EXES) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) $(LIBS) $(SYS_LIBS) $(LINKER)
	@cd $(DIR_OUTPUT) && $(SHELL) download.sh $(EXES)
	@echo Make $(OUTPUT_EXES) success!

archive:$(OBJS_LS) $(OBJS_BS) $(OBJS_C) 
	@if [ ! -d $(LIB_DIR) ]; then mkdir $(LIB_DIR); fi
	@if [ -f $(LIB_DIR)/*.a ]; then rm -f $(LIB_DIR)/*.a; fi
	@if [ -f $(INCLUDE_LIB_PATH)/$(ARCHIVE) ]; then rm -f $(INCLUDE_LIB_PATH)/$(ARCHIVE); fi
	$(V) $(AR) $(AR_ARGS) $(LIB_DIR)/$(ARCHIVE) $(OBJS_C) $(OBJS_LS) $(OBJS_BS) 
	@cp $(LIB_DIR)/*.a $(INCLUDE_LIB_PATH)
	@echo Make $(ARCHIVE) success!

clean:
	@ -rm -rf $(INCLUDE_LIB_PATH)/$(ARCHIVE)
	@ -rm -rf $(OUTPUT_EXES)
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
 
 
