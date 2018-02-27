#!/bin/sh
GREP=/cygdrive/d/cygwin64/bin/grep
AWK=/cygdrive/d/cygwin64/bin/awk
FIND=/cygdrive/d/cygwin64/bin/find
ECHO=/cygdrive/d/cygwin64/bin/echo
SORT=/cygdrive/d/cygwin64/bin/sort

#-----------------------Parser Makefile---------------------------------
OBJ=Makefile
if [ ! -f $OBJ ]; then
    echo "No target Makefile for parser"
    exit;
fi

TARGET_CPU=$(${GREP} 'export SoC' $OBJ | ${AWK} -F= '{print $2}')
TARGET_DIR=apps

DIR=.
#-----------------------Create Clang Completer Database---------------------------------
${ECHO} reloading Dir :$TARGET_DIR / CPU :$TARGET_CPU project ...

${ECHO} "Syncing ... apps";
# bear make -j dry_run > /dev/null 2>&1;

# /opt/utils/remove_db_target compile_commands.json compile_commands.json

#-----------------------Project Dir switch---------------------------------
${FIND} ${DIR} -path "${DIR}/${TARGET_DIR}/cpu/*" ! -path "${DIR}/${TARGET_DIR}/cpu/${TARGET_CPU}*" -prune -o \
    -path "${DIR}/${TARGET_DIR}/include/*" ! -path "${DIR}/${TARGET_DIR}/include/${TARGET_CPU}*" -prune -o \
    -path "${DIR}/include_lib/cpu/*" ! -path "${DIR}/include_lib/cpu/${TARGET_CPU}*" -prune -o \
    -path "${DIR}/libs/cpu/*" ! -path "${DIR}/libs/cpu/${TARGET_CPU}*" -prune -o \
    -path "${DIR}/{TARGET_DIR}/freertos/Common/Full*" -prune -o \
    -path "${DIR}/tools*" -prune -o \
    -path "${DIR}/undodir*" -prune -o \
    -name "*.[Sch]" -print > cscope.files

# ----------- lookup file
export FIND_EXCEPT="${FIND} . -path "*${DIR}/${TARGET_DIR}/cpu/*" ! -path "*${DIR}/${TARGET_DIR}/cpu/${TARGET_CPU}*" -prune -o \
    -path "*${DIR}/${TARGET_DIR}/include/*" ! -path "*${DIR}/${TARGET_DIR}/include/${TARGET_CPU}*" -prune -o \
    -path "*${DIR}/include_lib/cpu/*" ! -path "*${DIR}/include_lib/cpu/${TARGET_CPU}*" -prune -o \
    -path "*${DIR}/libs/cpu/*" ! -path "*${DIR}/libs/cpu/${TARGET_CPU}*" -prune -o \
    -path "*${DIR}/${TARGET_DIR}/post_build/*" ! -path "*${DIR}/${TARGET_DIR}/post_build/${TARGET_CPU}*" -prune -o \
    -path "${DIR}/{TARGET_DIR}/freertos/Common/Full*" -prune -o \
    -path "*${DIR}/tools*" -prune -o \
    -path "*${DIR}/undodir*" -prune -o"

# export FIND_EXCEPT="${FIND} . ! -path "*br16*" ! -path "*br17*" ! -path "*undodir*"" 

export FORMAT='.*\.\(c\|h\|s\|S\|ld\|s51\|lst\|map\)'

ctags -R --fields=+lS --languages=+Asm 

# ----------- cscope
cscope -bR

# ----------- lookup file
${ECHO} -e "!_TAG_FILE_SORTED\t2\t/2=foldcase/">filenametags

# echo ${FIND_EXCEPT} >> log.txt

${FIND_EXCEPT} -regex ${FORMAT} -type f -printf "%f\t%p\t1\n" | ${SORT} -f>>filenametags
