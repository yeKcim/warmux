#!/bin/bash

#Commented during tests for performace
rm -Rf install_dir
make install DESTDIR=$(pwd)/install_dir

rm -Rf SPackage
mkdir SPackage
mkdir SPackage/data
pushd $(pwd)/install_dir/usr/local/share/wormux
	tar c . |tar x -C ../../../../../SPackage/data
popd
mkdir SPackage/data/locale
pushd $(pwd)/install_dir/usr/local/share/locale
	tar c . |tar x -C ../../../../../SPackage/data/locale
popd
mkdir SPackage/bin/
cp $(pwd)/install_dir/usr/local/bin/wormux SPackage/bin/
mkdir SPackage/lib/
for i in $(ldd SPackage/bin/wormux |awk -F '[>(]+' '{print $2}' |grep -v ')');do
	cp $i SPackage/lib/
done
cp /lib/ld-linux.so.2 SPackage/lib/
#This dynamic loading stuff drives me crazy...

for i in $(strings SPackage/lib/libSDL_image*.so* |grep -E 'lib(.*).so');do
	cp /usr/lib/${i} SPackage/lib
done
#Would be great to have a cleanest way to get these ones
cp /usr/lib/libXrender.so.1	SPackage/lib
cp /usr/lib/libXrandr.so.2	SPackage/lib
cp /usr/lib/libXcursor.so.1	SPackage/lib
cp /usr/lib/libXfixes.so.3	SPackage/lib
#That was all dynamic loaded librarries... I hope !

strip SPackage/lib/*
strip SPackage/bin/wormux
cat > SPackage/wormux.sh << EOF
#!/bin/sh

LD_LIBRARY_PATH=./lib WORMUX_DATADIR=./data/ WORMUX_LOCALEDIR=./data/locale WORMUX_FONT_PATH=./data/font/DejaVuSans.ttf ./lib/ld-linux.so.2 ./bin/wormux -lb219.org
EOF
#You thaught what you read was the most awful thing you've ever seen?
#Here is even better
REV=$(svn info --xml |grep revis |awk -F '"' '{print $2}' |head -n 1)
DIR="wormux-0.8.4svn-r${REV}"
rm -Rf $DIR
mv SPackage $DIR
tar czf ${DIR}.tar.gz $DIR
#Wait i'm not done yet, here is the worst part:
cat > ${DIR}.sh << EOF
#!/bin/sh

DIR=\$(mktemp -d)
mkdir -p \${DIR} 2>&1 > /dev/null
cat \$0 |tail -n +11 |tar xz -C \${DIR}
cd \${DIR}/worm*
sh ./wormux.sh
cd -
rm -Rf \${DIR}
exit 0
EOF
#If you've understood the previous part, it won't shock you too much....
#Else you'll need some drugs to survive.
cat ${DIR}.tar.gz >> ${DIR}.sh
