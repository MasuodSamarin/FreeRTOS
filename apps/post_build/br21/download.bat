@echo off

@echo ********************************************************************************
@echo 			SDK BR21 FLASH  ...			
@echo ********************************************************************************
@echo %date%

cd %~dp0

if exist %1.bin del %1.bin 
if exist %1.lst del %1.lst 

echo %1
echo %2
echo %3

REM %2 -disassemble %1.exe > %1.lst 
%3 -O binary -j .text  %1.exe  %1.bin 
%3 -O binary -j .data  %1.exe  data.bin 
%3 -O binary -j .nvdata %1.exe  nvdata.bin 
%2 -section-headers  %1.exe 

copy %1.bin/b + data.bin/b + nvdata.bin/b sdk.app

REM if exist %1.exe del %1.exe
del %1.bin data.bin nvdata.bin


if exist uboot.boot del uboot.boot
type uboot.bin > uboot.boot


isd_download.exe -tonorflash -dev br21 -boot 0x2000 -div6 -wait 300 -f uboot.boot sdk.app bt_cfg.bin 


::-format cfg tone_resource_mini/test.mp3

:: -read flash_r.bin 0-2M

if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist jl_690x.bin del jl_690x.bin
if exist jl_cfg.bin del jl_cfg.bin


::rename jl_isd.bin jl_690x.bin
::bfumake.exe -fi jl_690x.bin -ld 0x0000 -rd 0x0000 -fo jl_690x.bfu

@rem rename jl_isd.bin jl_cfg.bin
@rem bfumake.exe -fi jl_cfg.bin -ld 0x0000 -rd 0x0000 -fo jl_690x.bfu
@rem copy /b jl_690x.bfu+jl_flash_cfg.bin  jl_690x.bfu

REM  IF EXIST no_isd_file del jl_690x.bin jl_cfg.bin
REM  del no_isd_file


@rem format vm        //擦除VM 68K区域
@rem format cfg       //擦除BT CFG 4K区域
@rem format 0x3f0-2  //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

ping /n 2 127.1>null
::pause

