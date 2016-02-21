#!/bin/sh

# Clean up
./clean.sh

#-----------------------------------------------------------------------------

find_tool_version()
{
    TOOL=$1
    MINVER=$2
    OKVER=$3
    NOKREG=$4

    for i in ${OKVER}; do
        if ${TOOL}${i} --version > /dev/null 2>&1; then
            echo "${TOOL}${i}"
            exit
        fi
    done
    if ${TOOL} --version > /dev/null 2>&1; then
        case "$(${TOOL} --version | sed -e '1s/[^0-9]*//' -e q)" in
            ${NOKREG})
            echo "You need ${TOOL} version (at least) ${MINVER} !" 1>&2
            exit 1;;
        esac
    else
        echo "You need ${TOOL} !" 1>&2
        exit 1
    fi
    echo "${TOOL}"
}

#-----------------------------------------------------------------------------

AUTOMAKE_MIN_VERSION="1.6"
AUTOMAKE_OK_VERSIONS="-1.9 19 -1.8 18 -1.7 17 -1.6 16 -1.5 15"
AUTOMAKE_NOK_REGEXP='0|0.*|1|1.[0-5]*'

AUTOCONF_MIN_VERSION="2.58"
AUTOCONF_OK_VERSIONS="258 259"
AUTOCONF_NOK_REGEXP='0|0.*|1|1.*|2|2.[0-4]*|2.5[0-7]*'

AUTOMAKE=$(find_tool_version automake "${AUTOMAKE_MIN_VERSION}" \
                                      "${AUTOMAKE_OK_VERSIONS}" \
                                      "${AUTOMAKE_NOK_REGEXP}") || exit 1
ACLOCAL=$(find_tool_version aclocal "${AUTOMAKE_MIN_VERSION}" \
                                    "${AUTOMAKE_OK_VERSIONS}" \
                                    "${AUTOMAKE_NOK_REGEXP}") || exit 1
AUTOCONF=$(find_tool_version autoconf "${AUTOCONF_MIN_VERSION}" \
                                      "${AUTOCONF_OK_VERSIONS}" \
                                      "${AUTOCONF_NOK_REGEXP}") || exit 1
AUTOHEADER=$(find_tool_version autoheader "${AUTOCONF_MIN_VERSION}" \
                                          "${AUTOCONF_OK_VERSIONS}" \
                                          "${AUTOCONF_NOK_REGEXP}") || exit 1

#-----------------------------------------------------------------------------

echo "[+] Run aclocal"
if [ -d /usr/local/share/aclocal ]; then
    $ACLOCAL -I m4 -I /usr/local/share/aclocal || exit 1
else
    $ACLOCAL -I m4 || exit 1
fi

echo "[+] Run autoheader"
$AUTOHEADER || exit 1

echo "[+] Run automake"
touch config.rpath # automake 1.10 has a bug and wants this file
$AUTOMAKE --add-missing --copy || exit 1

echo "[+] Run autoconf"
$AUTOCONF || exit 1

echo
echo "Now run ./configure"

