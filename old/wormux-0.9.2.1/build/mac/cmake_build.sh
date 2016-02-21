#!/bin/sh

#################################################
#  Script to compile with cmake under MacOS X   #
#           For     W O R M U X                 #
#           done by Plorf, Auria,               #
#         lynxlynxlynx and Snaggle              #
#################################################

# run ./cmake_build.sh --help for more info on how to use


#
# Set environment for compile
#

MAC=`pwd`/
ROOT=${MAC}../../
SRC=${ROOT}src/


# we are building x86 all the time for now. since we want backward compatibility even when building
# on 10.6, use GCC 4.0.
export CC=gcc-4.0
export CXX=g++-4.0
export LD=g++-4.0

# used for non-universal builds (universal builds change this to 10.4)
export MACOSX_DEPLOYMENT_TARGET=10.5

# flags used when universal mode is off
export COMPAT_FLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.4"
export COMPAT_LD_FLAGS="-arch i386 -mmacosx-version-min=10.4 -Wl,-syslibroot,/Developer/SDKs/MacOSX10.5.sdk"
export CMAKE_OSX_SYSROOT="/Developer/SDKs/MacOSX10.5.sdk"

# flags used for all kinds of builds
export INCLUDES="-I/Library/Frameworks/SDL.framework/Versions/A/Headers/ -I/Library/Frameworks/wmxlibpng.framework/Versions/A/Headers/ -I/Library/Frameworks/SDL_net.framework/Versions/A/Headers/ -I/usr/local/include/"

export UNIVERSAL_BUILD=0
export PPC_BUILD=0

# flags only used when building as UB
export FAT_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386 -I/Developer/SDKs/MacOSX10.4u.sdk/usr/include -mmacosx-version-min=10.4"
export FAT_LDFLAGS="-mmacosx-version-min=10.4 -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch ppc -arch i386 -L/Developer/SDKs/MacOSX10.4u.sdk/usr/lib"

# flags only used when cross-building to PCC
export PPC_CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch ppc -I/Developer/SDKs/MacOSX10.4u.sdk/usr/include -mmacosx-version-min=10.4"
export PPC_LDFLAGS="-mmacosx-version-min=10.4 -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch ppc -L/Developer/SDKs/MacOSX10.4u.sdk/usr/lib"

APP_VERSION=0.8.3
BUNDLE_NAME=Wormux
DMG_TARGET="${BUNDLE_NAME}-${APP_VERSION}"
DMG_OUT=${BUNDLE_NAME}-${APP_VERSION}-`uname -p`


if [ "$1" = "--help" ]
then
    echo "This script builds Wormux on Mac OS X by using the CMake build system"
    echo "Requirements :"
    echo "  - CMake 2.6 +"
    echo "  - libintl (gnu gettext) headers [a universal binary of the library is included"
    echo "                                   along this script but not the headers]"
    echo "  - the mac dependencies package (http://download.gna.org/wormux/mac/mac_dependencies.zip)"
    echo "    installed in your /Library/Frameworks directory."
    echo ""
    echo "targets :"
    echo "./cmake_build              : default build (i386, compatible OS X 10.5+)"
    echo "./cmake_build    universal : build a universal app"
    echo "./cmake_build.sh ppc       : cross-compile to PPC"
    echo ""
    echo "arguments :"
    echo "-j<x> : launch make with x threads"

    exit 1
fi



if [ $# = 1 ]
then
    if [ "$1" = "universal" ]
    then
        UNIVERSAL_BUILD=1
    elif [ "$1" = "ppc" ]
    then
        PPC_BUILD=1
    else
        NBTHREADS=$1
        echo "Launch with ${NBTHREADS} !"
    fi
fi


if [ $# = 2 ]
then
    if [ "$1" = "universal" ]
    then
        TMP1=${1}
        TMP2=${2}
        UNIVERSAL_BUILD=1
    elif [ "$1" = "ppc" ]
    then
        TMP1=${1}
        TMP2=${2}
        PPC_BUILD=1
    else
        TMP2=${1}
        TMP1=${2}
    fi
        
    NBTHREADS=$TMP2
    echo "Launch with ${NBTHREADS} !"
fi

if [ ${UNIVERSAL_BUILD} = 1 ]; then
    echo "*******************************"
    echo "Universal build mode enabled !"
    echo "*******************************"
    echo ""
        
    export MACOSX_DEPLOYMENT_TARGET=10.4
    export CMAKE_OSX_ARCHITECTURES="ppc;i386"
    export CMAKE_OSX_SYSROOT="/Developer/SDKs/MacOSX10.4u.sdk"

    DMG_OUT=${BUNDLE_NAME}-${APP_VERSION}-Universal
    
    export CFLAGS="${FAT_CFLAGS} ${CFLAGS} ${INCLUDES}"
    export CXXFLAGS="${CFLAGS}"
    export LDFLAGS="${FAT_LDFLAGS} ${LDFLAGS}"
elif [ ${PPC_BUILD} = 1 ]; then
    echo "************************"
    echo "PPC build mode enabled !"
    echo "************************"
    echo ""
        
    export MACOSX_DEPLOYMENT_TARGET=10.4
    export CMAKE_OSX_ARCHITECTURES=ppc
    export CMAKE_OSX_SYSROOT="/Developer/SDKs/MacOSX10.4u.sdk"

    DMG_OUT=${BUNDLE_NAME}-${APP_VERSION}-PPC
    
    export CFLAGS="${PPC_CFLAGS} ${CFLAGS} ${INCLUDES}"
    export CXXFLAGS="${CFLAGS}"
    export LDFLAGS="${PPC_LDFLAGS} ${LDFLAGS}"
else
    export CFLAGS="${CFLAGS} ${INCLUDES} ${COMPAT_FLAGS}"
    export CXXFLAGS="${CFLAGS} ${COMPAT_FLAGS}"
    export LDFLAGS="${LDFLAGS} ${COMPAT_LD_FLAGS}"
fi

#
# Set files for CMake and compilation
#

# Copy library libSDLmain_UB.a
if [ -e libSDLmain_UB.a ]
then
    cp libSDLmain_UB.a ${ROOT}
else
    echo "*** ${MAC}libSDLmain_UB.a Not Found ***"
    exit
fi

# Copy library libintl.a
if [ -e libintl.a ]
then
    cp libintl.a ${ROOT}
else
    echo "*** ${MAC}libintl.a Not Found ***"
    exit
fi


#
# Clean temps files, and create Folders
#

TMP=${MAC}tmpbuild/
#if [ -e ${TMP} ]
#then
#    echo "*****************"
#    echo "Clean tmpbuild"
#    rm -rf ${TMP}
#    echo "*****************"
#fi
mkdir -p ${TMP}

APP=${MAC}Wormux.app
if [ -e ${APP} ]
then
    echo "******************"
    echo "Clean package Wormux.app"
    rm -rf ${APP}
    echo "******************"
fi

if [ -e ${DMG_OUT}.app ]
then
    echo "******************"
    echo "Clean package ${DMG_OUT}.app"
    rm -rf ${DMG_OUT}.app
    echo "******************"
fi

if [ -e ${DMG_OUT}.dmg ]
then
    echo "******************"
    echo "Clean package ${DMG_OUT}.dmg"
    rm -rf ${DMG_OUT}.dmg
    echo "******************"
fi

echo "Create Wormux.app tree"
mkdir -p ${APP}/Contents/MacOS/
mkdir -p ${APP}/Contents/Frameworks/
RES=${APP}/Contents/Resources/
mkdir -p ${RES}data/
mkdir -p ${RES}locale/

# Add icon and info.plist and PkgInfo
cp ${MAC}Info.plist.in ${APP}/Contents/Info.plist
cp ${MAC}PkgInfo.in ${APP}/Contents/PkgInfo
cp ${ROOT}data/wormux_128x128.icns ${RES}Wormux.icns


#export CMAKE_INSTALL_PREFIX=${RES}

#
# Compile
#

cd ${TMP}
echo "Configuring CMake build"

if ! cmake ${ROOT} --graphviz=viz.dot -DDATA_PATH=${RES} -DBIN_PATH=${APP}/Contents/MacOS/ -DBUILD=Release -DPREFIX=${RES} #-DGETTEXT_LIBRARY="../../libintl.a"
then
    echo "CMake error"
    exit 1
fi

if ! make VERBOSE=1 -j2
then
    echo "make error"
    exit 1
fi
if ! make install
then
    echo "make install error"
    exit 1
fi


#
# Copy resources inside bundle
#


mv ${RES}/share/locale ${RES}
rm -rf ${RES}/share

# Do a simple test for check if data is well copied
if [ -e ${RES}/data/wormux_default_config.xml ]
then
    echo "Default_config ok"
else
    echo "*** ERROR : No Default_config ! : Probably no datas copied ***"
    exit 2 
fi

# tell the Finder the app was updated
touch ${APP}

#
# Copy frameworks into package
#

#echo "Copy all frameworks"
#cd ${MAC};

# If frameworks are not available, they'll be download from this mirror
#MIRROR=http://plorf.homeip.net/wormux/lib/
#if [ ! -e "${MAC}frameworks.tar.bz2" ]
#then 
#    echo "Frameworks will be downloaded from ${MIRROR} (3MB)";
#    curl ${MIRROR}frameworks.tar.bz2 -o ${MAC}frameworks.tar.bz2;
#fi
#    tar xfj ${MAC}frameworks.tar.bz2 -C ${APP}/Contents/Frameworks;
#    echo "Frameworks copy done"




#
# Clean environment
#

# Remove copy of libSDLmain_UB.a
#if [ -e ${ROOT}libSDLmain_UB.a ]
#then
#    rm ${ROOT}libSDLmain_UB.a
#fi

# Remove copy of libintl.a
#if [ -e ${ROOT}libintl.a ]
#then
#    rm ${ROOT}libintl.a
#fi

echo "Build done"

exit 0