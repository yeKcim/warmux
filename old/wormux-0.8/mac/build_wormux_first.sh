export CFLAGS=-I/sw/include 
export LDFLAGS=-L/sw/lib 
export CXXFLAGS=$CFLAGS 
export CPPFLAGS=$CXXFLAGS 
export ACLOCAL_FLAGS="-I /sw/share/aclocal"
export PKG_CONFIG_PATH="/sw/lib/pkgconfig"
export MACOSX_DEPLOYMENT_TARGET=10.4

export CP=/bin/cp
export HDIUTIL=/usr/bin/hdiutil
export INSTALL=/usr/bin/install
export INSTALL_NAME_TOOL=/usr/bin/install_name_tool
export LS=/bin/ls
export MAKE=/usr/bin/make
export OTOOL=/usr/bin/otool
export RM=/bin/rm

#cd wormux
cd ..
svn up
make clean
./autogen.sh
./configure --enable-debug
make -w



echo "*******************************************"
echo "lauch build_wormux_(fink/macports) script !"
echo "*******************************************"
cd mac 
# Macports / Fink
# Fink : comment the second line
# Macports : comment the first line
./build_wormux_fink.sh
#./build_wormux_macports.sh
