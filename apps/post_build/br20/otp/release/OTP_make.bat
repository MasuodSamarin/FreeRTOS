@echo off&setlocal enabledelayedexpansion
title OTP Download by:BR20 2017ƒÍ7‘¬13»’

@echo ***************************************************
@echo 			BR20 OTP loading ...			
@echo ***************************************************
@echo %date%

cd %~dp0

if exist %1.bin del %1.bin 
if exist %1.lst del %1.lst 

REM  echo %1
REM  echo %2
REM  echo %3

REM %2 -disassemble %1.exe > %1.lst 
%3 -O binary -j .text  %1.exe  %1.bin 
%3 -O binary -j .data  %1.exe  data.bin 
%3 -O binary -j .nvdata %1.exe  nvdata.bin 
%2 -section-headers  %1.exe 

copy %1.bin/b + data.bin/b + nvdata.bin/b sdk.app

REM if exist %1.exe del %1.exe
del %1.bin data.bin nvdata.bin

@br20_config.exe -boot 0x2000 -f otp_uboot.bin sdk.app otp_cfg.bin user_cfg.bin -user_otp_adr 0xfe00

@bin2mif -b 0x00 -l 0x8000 -e -w 32 -i otp.bin -o otp0.mif
@bin2mif -b 0x8000  -l 0x10000 -e -w 32 -i otp.bin -o otp1.mif

ping /n 2 127.1>null
::pause
exit


mode con lines=14 cols=55
set z=0
SET ge=0
for /l %%i in (1,1,25) do (
  set "a%%i=  "
  set "c=!c!®Å"
  set "d=!d! "
)
:1
set /a z+=4
for /l %%i in (1,1,3) do ping /n 1 127.1>nul
set /a ge+=1
set "a%ge%=®Å"
set jdt=
for /l %%i in (1,1,25) do set "jdt=!jdt!!a%%i!"
cls
@echo ***************************************************
@echo 			BR20 OTP loading ...			
@echo ***************************************************
@echo %date%
echo ©≥©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©∑
echo ©ß%jdt%©ß
echo ©ª©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©•©ø
echo %d:~1%%z%©á
if "%jdt%"=="%c%" echo OK^! & exit
goto 1

::pause
::"C:\Program Files\AutoHotkey\AutoHotkey.exe" mouse.ahk
