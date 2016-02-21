#!/bin/bash
# This snippet will find all the source files that are not mentioned in
# po/Makefile.am (effectively rendering any translation in them a noop).
# Then it puts them into po/Makefile.am;
[[ -d po ]] || { echo "Run me from the base wormux dir!"; exit 1; }

csplit po/Makefile.am "/^POTFILES =/"
rm xx00 
sed -n '1, /^POTFILES =/p' po/Makefile.am > new.file
find src/ tools/ \( -name *.cpp -o -name *.h \) -printf '        %p \\\n' | sort >> new.file
sed -i '/^POTFILES =/,/^\s*$/d' xx01
sed -i '$ s,\\$,,' new.file
echo >> new.file
cat new.file xx01 > po/Makefile.am
rm new.file xx0*
