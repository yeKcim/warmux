#!/bin/sh
APP=wormux
LOG=valgrind.log
valgrind \
    --verbose \
    --show-reachable=yes \
    --log-file-exactly=$LOG \
    --leak-check=full \
    --run-libc-freeres=no \
    $APP
