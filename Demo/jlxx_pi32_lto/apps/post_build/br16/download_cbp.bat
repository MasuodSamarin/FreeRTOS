
@echo off

if "%1" == "pi32" (
    echo "download pi32" %1 %2
    set OBJDUMP = ../../../target/pi32/bin/pi32-objdump
    set OBJCOPY = ../../../target/pi32/bin/pi32-objcopy
    )

if "%1" =="pi32_lto" (
    echo "download pi32_lto" %1  %2
    set OBJDUMP = ../../../target_lto/pi32/bin/pi32-objdump
    set OBJCOPY = ../../../target_lto/pi32/bin/pi32-objcopy
    )

cd tools/%2
call download.bat %2 %OBJDUMP% %OBJCOPY% 
