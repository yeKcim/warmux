#!/bin/sh

mkdir tmpbuild
cd tmpbuild
export CMAKE_INSTALL_PREFIX=./wormux-files
cmake ../..
make
sudo make install

# pwd = /mac/tmpbuild
rm -rf ../Wormux.app
echo "Create Wormux.app file"
mkdir -p ../Wormux.app/Contents/Resources
mkdir -p ../Wormux.app/Contents/MacOS
mkdir -p ../Wormux.app/Contents/Frameworks

echo "Copy data into .app file"

# Copy data files into .app
mkdir -p ../Wormux.app/Contents/Resources/data
echo "Check data copy"
cp -R /usr/local/share/wormux ../Wormux.app/Contents/Resources/data
#echo "Error : i think to much data choose.. (140MB at end)"
#cp -R ../../data/ ../Wormux.app/Contents/Resources/data/

# Copy locale files into .app
mkdir -p ../Wormux.app/Contents/Resources/locale
#echo "Error : wormux in english, so i think it's not working... search where is it"
#cp -R /usr/share/locale ../Wormux.app/Contents/Resources/locale
echo "Check locale copy"
cp -R /usr/local/share/locale ../Wormux.app/Contents/Resources/locale

# pwd = /mac
cd ..

# Add icon and info.plist and PkgInfo
cp Info.plist.in ./Wormux.app/Contents/info.plist
cp PkgInfo.in ./Wormux.app/Contents/
cp ../data/wormux_128x128.icns ./Wormux.app/Contents/Resources/Wormux.icns


#pwd = /mac/tmpbuild
cd tmpbuild

echo "Add bin"
cp src/wormux ../Wormux.app/Contents/MacOS/wormux

echo "Add data"
#cp -r $INSTALL_PREFIX/share/wormux ./Wormux.app/Contents/Resources/data
cp -r /usr/local/share/locale ../Wormux.app/Contents/Resources/locale

#pwd = /mac
cd ..

echo "COPY DYLIB OR FRAMEWORKS ????"
echo "Check if write rights are ok to copy frameworks"
echo "Copy all frameworks"

echo "Remove temps files"
#rm -rf tmpbuild
echo "Build done"

