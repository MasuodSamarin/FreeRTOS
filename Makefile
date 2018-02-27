# -----------------Usage --------------------------

# 1.编译apps:		make;
# 2.清理apps:		make clean;
# 3.编译全部libs:	make libs;
# 4.清理全部libs:	make clean_libs;
# 5.编译单独lib:	make lib target=...;
# 6.清理单独lib:	make clean_lib target=...;

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

#配置编译芯片平台

.PHONY: all clean lib clean_lib libs clean_libs usage

ifeq ($(HOST_OS), windows)
all:pre_make
	@make -C $(TARGET) || exit 1

clean:
	@make clean -C $(TARGET) || exit 1

lib:
	@make -C libs\$(TARGET) || exit 1

clean_lib:
	@make clean -C libs\$(target) || exit 1

libs:
	@for /f %%i in ('dir $(LIBS_PATH) /ad /b') do if exist libs\%%i\Makefile make archive -C libs\%%i || exit 1

clean_libs:
	@for /f %%i in ('dir $(LIBS_PATH) /ad /b') do if exist libs\%%i\Makefile make clean -C libs\%%i || exit 1

endif

ifeq ($(HOST_OS), linux)

all:pre_make
	@if [ -f $(TARGET)/cpu/$(RELEASE)/version.z ]; then $(VER) $(TARGET)/cpu/$(RELEASE)/version.z $(TARGET)/cpu/$(RELEASE)/version.S; fi ;
	@make -C $(TARGET) || exit 1

clean:
	@make clean -C $(TARGET) || exit 1

lib:
	@if [ -f libs/$(TARGET)/version.z ]; then $(VER) libs/$(TARGET)/version.z libs/$(TARGET)/version.S; fi
	@make archive -C libs/$(TARGET) || exit 1

clean_lib:
	@make clean -C libs/$(TARGET) || exit 1

libs:
	@for i in $(MAKEDIR); do \
		if [ -f $$i/version.z ]; then $(VER) $$i/version.z $$i/version.S; fi ;\
		if [ -f $$i/$(PLATFORM)/version.z ]; then $(VER) $$i/$(PLATFORM)/version.z $$i/$(PLATFORM)/version.S; fi ;\
		if [ $$i != "." ] && [ -f $$i/Makefile ]; then \
			make archive -C $$i || exit 1; \
		fi \
	done

clean_libs:
	@echo $(MAKEDIR)
	@for i in $(MAKEDIR); do \
		if [ -f $$i/version.S ]; then rm -f $$i/version.S; fi ;\
		if [ -f $$i/$(PLATFORM)/version.S ]; then rm -f $$i/$(PLATFORM)/version.S; fi ;\
		if [ $$i != "." ] && [ -f $$i/Makefile ]; then \
			make clean -C $$i || exit 1; \
		fi \
	done

dry_run:
	@for i in $(MAKEDIR); do \
		if [ $$i != "." ] && [ -f $$i/Makefile ]; then \
			make clean -C $$i || exit 1; \
			make dry_run -C $$i || exit 1; \
			make clean -C $$i || exit 1; \
		fi \
	done
	@make clean -C $(TARGET) || exit 1
	@make dry_run -C $(TARGET) || exit 1
	@make clean -C $(TARGET) || exit 1

endif

usage:
	@echo Complie apps : make
	@echo Clean apps   : make clean
	@echo Complie libs : make libs
	@echo Clean libs   : make clean_libs
	@echo Complie lib  : make lib TARGET=...
	@echo Clean lib    : make clean_lib TARGET=...

-include tools$(SLASH)platform$(SLASH)Makefile.$(SoC)

