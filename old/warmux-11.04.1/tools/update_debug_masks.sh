#!/bin/sh

sourcefiles=`find src/ -name "*.h" -or -name "*.cpp"`
if [ "$sourcefiles" = "" ]; then
    echo "error searching the files"
    exit 1
fi

masks1=`grep -ho 'MSG_DEBUG[[:space:]]*("[^"]*' $sourcefiles | cut -d\" -f2`
masks2=`grep -ho 'IsLOGGING[[:space:]]*("[^"]*' $sourcefiles | cut -d\" -f2`

MASKS=`echo $masks1 $masks2 | tr " " "\n" | sort -u`
MASKS2=`echo $MASKS | sed "s/ / | /g"`
sed -i "s/std::string used_debug_masks.*/std::string used_debug_masks = \"$MASKS2\";/" src/include/debugmasks.h
