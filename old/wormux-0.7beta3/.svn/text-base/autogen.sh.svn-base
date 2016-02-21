#! /bin/sh

# Clean up
./clean.sh

#-----------------------------------------------------------------------------

function find_tool_version
{
    TOOL=$1
    for i in 1.9 1.8 1.7 1.6 1.5; do
        if $TOOL-$i --version >/dev/null 2>&1; then
            echo "$TOOL-$i"
            exit
        fi
    done
    if $TOOL --version > /dev/null 2>&1; then
    case "$($TOOL --version | sed -e '1s/[^0-9]*//' -e q)" in
        0|0.*|1|1.[0-4]|1.[0-4][-.]*) 
        echo "You need $TOOL version (at least) 1.5 !" 1>&2
        exit 1;;
    esac
    fi
    echo "$TOOL"
}

#-----------------------------------------------------------------------------

AUTOMAKE=$(find_tool_version automake 1.5) || exit 1
ACLOCAL=$(find_tool_version aclocal 1.5) || exit 1
AUTOCONF=autoconf

#-----------------------------------------------------------------------------

echo "Run aclocal"
$ACLOCAL -I m4 || exit 1

autoheader || exit 1

echo "Run automake"
$AUTOMAKE --add-missing --copy || exit 1

echo "Run autoconf"
$AUTOCONF || exit 1

echo
echo "Now run ./configure"

