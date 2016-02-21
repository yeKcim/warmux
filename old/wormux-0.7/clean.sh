#! /bin/sh

echo "Clean autogen things"
rm -f config.log config.status mkinstalldirs configure aclocal.m4
rm -rf autom4te.cache
rm -rf src/.deps
find -name "Makefile" -exec rm {} \;
for i in $(find -name "Makefile.in" | grep -v '^./intl/'); do rm "$i"; done
