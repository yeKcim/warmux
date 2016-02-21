#!/bin/sh
if [ "$1" = "" ]; then
    echo "Usage: $0 /path/to/wormux"
    exit
fi

APP=$1
LOG=valgrind.log
valgrind \
    --verbose \
    --show-reachable=yes \
    --log-file-exactly=$LOG \
    --leak-check=full \
    --run-libc-freeres=no \
    $APP
