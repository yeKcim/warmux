#!/bin/sh
if [ "$1" = "" ]; then
    echo "Usage: $0 <valgrind options> /path/to/warmux <warmux options>"
    exit
fi

OPTIONS="--show-reachable=yes --leak-check=full --leak-resolution=high"
if [ "$1" != "src/warmux" ]; then OPTIONS=""; fi

GLIBCXX_FORCE_NEW=1 valgrind \
  --tool=memcheck --log-file=valgrind.log --num-callers=12 \
  --suppressions=$(dirname $0)/warmux.supp $OPTIONS $* -f
