#!bin/bash

if [ -f $1.bin ]; then rm $1.bin; fi
if [ -f $1.lst ]; then rm $1.lst; fi

${OBJDUMP} -D $1.exe > $1.lst
${OBJCOPY} -O binary -j .text  $1.exe  $1.bin
${OBJCOPY} -O binary -j .data  $1.exe  data.bin
${OBJCOPY} -O binary -j .nvdata $1.exe  nvdata.bin
${OBJDUMP} -section-headers  $1.exe
# ${OBJDUMP} -t $1.or32 > $1.symbol.txt

cat $1.bin data.bin nvdata.bin > sdk.app 

# host-client -project sdk_br16 -f uboot.boot sdram.app
host-client -project ${NICKNAME}_${CODE_SPACE}_debug -f ../../flash/uboot.bin ver.bin sdk.app ../../flash/${PLATFORM}loader.bin

