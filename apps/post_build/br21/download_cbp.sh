#!bin/bash

export OBJCOPY
export OBJDUMP


if [ $1 = "pi32" ]
then 
    echo "download pi32" $2
    JL_TOOLS=/opt/pi32
    OBJCOPY=${JL_TOOLS}/bin/pi32-objcopy
    OBJDUMP=${JL_TOOLS}/bin/pi32-objdump
fi

if [ $1 = "pi32_lto" ]
then 
    echo "download pi32_lto" $2
    JL_TOOLS=/opt/pi32/test
    OBJCOPY=${JL_TOOLS}/bin/pi32-objcopy
    OBJDUMP=${JL_TOOLS}/bin/pi32-objdump
fi

cd tools/$2
${SHELL} download.sh $2
