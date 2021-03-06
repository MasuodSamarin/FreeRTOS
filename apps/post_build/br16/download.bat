@echo off

@echo ********************************************************************************
@echo 			SDK BR16 download ...			
@echo ********************************************************************************
@echo %date%

cd %~dp0

if exist %1.bin del %1.bin 
if exist %1.lst del %1.lst 

REM  echo %1
REM  echo %2
REM  echo %3

%2 -disassemble %1.or32 > %1.lst 
%3 -O binary -j .text  %1.or32  %1.bin 
%3 -O binary -j .data  %1.or32  data.bin 
%3 -O binary -j .data1 %1.or32  data1.bin 
%2 -section-headers  %1.or32 

copy %1.bin/b + data.bin/b + data1.bin/b sdram.app

if exist %1.or32 del %1.or32
del %1.bin data.bin data1.bin

cd tone_resource
copy *.mp3 ..\
cd ..
cd ui_resource
copy *.* ..\
cd ..
cd cfg_resouce
copy *.bin ..\
cd ..

isd_download.exe -tonorflash -dev br16 -boot 0x2000 -div6 -wait 300 -f uboot.boot sdram.app  
::-format cfg
:: -read flash_r.bin 0-2M

if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist jl_461x.bin del jl_461x.bin
if exist jl_cfg.bin del jl_cfg.bin
if exist bt_cfg.bin del bt_cfg.bin

rename jl_isd.bin jl_461x.bin
bfumake.exe -fi jl_461x.bin -ld 0x0000 -rd 0x0000 -fo jl_461x.bfu
copy /b jl_461x.bfu+jl_flash_cfg.bin  jl_461x.bfu

@rem rename jl_isd.bin jl_cfg.bin
@rem bfumake.exe -fi jl_cfg.bin -ld 0x0000 -rd 0x0000 -fo jl_461x.bfu
@rem copy /b jl_461x.bfu+jl_flash_cfg.bin  jl_461x.bfu

IF EXIST no_isd_file del jl_461x.bin jl_cfg.bin
del no_isd_file

@rem format vm        //擦除VM 68K区域
@rem format cfg       //擦除BT CFG 4K区域
@rem format 0x3f0-2  //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)



