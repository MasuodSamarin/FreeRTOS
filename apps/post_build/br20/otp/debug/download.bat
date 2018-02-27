@echo off

@echo ********************************************************************************
@echo 			BR20 OTP Debug SDK download...			
@echo ********************************************************************************
@echo %date%

cd %~dp0

if exist %1.bin del %1.bin 
if exist %1.lst del %1.lst 

REM  echo %1
REM  echo %2
REM  echo %3

REM %2 -disassemble %1.elf > %1.lst 
%3 -O binary -j .text  %1.elf  %1.bin 
%3 -O binary -j .data  %1.elf  data.bin 
%3 -O binary -j .nvdata %1.elf  nvdata.bin 
%2 -section-headers  %1.elf 

copy %1.bin/b + data.bin/b + nvdata.bin/b sdk.app

REM if exist %1.elf del %1.elf
del %1.bin data.bin nvdata.bin

copy /y ..\..\flash\uboot.bin .
copy /y ..\..\flash\br20loader.bin .
copy /y ..\ver.bin .

if exist uboot.boot del uboot.boot
type uboot.bin > uboot.boot

..\..\flash\isd_download.exe -tonorflash -dev br20 -boot 0x2000 -div6 -wait 300 -format cfg -f uboot.boot sdk.app 




