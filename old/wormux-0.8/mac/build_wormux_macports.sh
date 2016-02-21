#!/bin/sh -e

# script to build a Mac OS X clickable .app

# To use this file, first install all the Wormux dependencies.
# Using Fink is a good way to manage the various libraries.
# You will need:
# glib2-dev glib2-shlibs glibmm2.4-dev glibmm2.4 libcurl4 libcurl4-shlibs
# libgettext3-dev libgettext3-shlibs gettext-tools libiconv-dev libiconv
# libpng3 libpng3-shlibs libsigc++2 libsigc++2-shlibs libxml++2 libxml++2-shlibs
# libxml2 libxml2-shlibs sdl sdl-shlibs sdl-gfx13 sdl-gfx13-shlibs
# sdl-image sdl-image-shlibs sdl-mixer sdl-mixer-shlibs
# sdl-net sdl-net-shlibs sdl-ttf sdl-ttf-shlibs
#
# There's a chance you might also need smpeg smpeg-shlibs libssh2.1
# libssh2.1-shlibs, but only install them if the wormux build complains
# about them.

# The svn package is also needed in order to download the wormux source

# To use, simply run this script from the parent directory where
# the wormux source code is kept (eg /src).
# cd /src
# sh ./build_wormux_svn.sh

export HERE=`pwd`

export PROG_NAME=wormux
export BUNDLE_NAME=Wormux
export APP_VERSION=0.8svn

if [ -d '/sw' ];
then
	EXTRA_PATH=/sw/bin:/sw/sbin
	EXTRA_CFLAGS=-I/sw/include
	EXTRA_LDFLAGS=-L/sw/lib
	EXTRA_ACLOCAL_FLAGS="-I /sw/share/aclocal"
	EXTRA_PKG_CONFIG_PATH="/sw/lib/pkgconfig"
elif [ -d '/opt/local' ];
then
	EXTRA_PATH=/opt/local/bin:/opt/local/sbin
	EXTRA_CFLAGS=-I/opt/local/include
	EXTRA_LDFLAGS=-L/opt/local/lib
	EXTRA_ACLOCAL_FLAGS="-I /opt/local/share/aclocal"
	EXTRA_PKG_CONFIG_PATH="/opt/local/lib/pkgconfig"
fi

export SOURCE_DIR=${HERE}/..
#export FINK_PATH=/sw/bin:/sw/sbin
#export  OTHER_PATH=/src/usr/local-${ARCH}/bin
#export MACPORTS_PATH=/opt/local/bin:/opt/local/sbin
export PATH=${EXTRA_PATH}:/bin:/sbin:/usr/bin:/usr/sbin
export CONFIGURE_PARAMS="--disable-sdltest"

export APP_DIR="${SOURCE_DIR}/${BUNDLE_NAME}.app"

export CNT_TARGET=${APP_DIR}/Contents

export MACOS_TARGET=${CNT_TARGET}/MacOS
export RSC_TARGET=${CNT_TARGET}/Resources



export CFLAGS=${EXTRA_CFLAGS}
export LDFLAGS=${EXTRA_LDFLAGS}
export CXXFLAGS=$CFLAGS 
export CPPFLAGS=$CXXFLAGS 
export ACLOCAL_FLAGS=${EXTRA_ACLOCAL_FLAGS}
export PKG_CONFIG_PATH=${EXTRA_PKG_CONFIG_PATH}
export MACOSX_DEPLOYMENT_TARGET=10.4


# Get the new source
echo ""
echo "Calling svn to fetch new/changed files."
echo ""

if [ ! -e ${SOURCE_DIR} ] ; then
	echo "Could not find the wormux source code";
	echo "I will download a new source tree using SVN";
	export OLD_SVN=2456;
#	svn co http://svn.gna.org/svn/wormux/trunk ${SOURCE_DIR};
	cd ${SOURCE_DIR};
	if [ ! -e data/wormux_128x128.icns ]; then
		/usr/bin/curl -o ${SOURCE_DIR}/data/wormux_128x128.icns "http://snaggledworks.com/software/wormux/wormux_128x128.icns";
		/usr/bin/curl -o ${SOURCE_DIR}/mac/Info.plist.in "http://snaggledworks.com/software/wormux/Info.plist.in";
		/usr/bin/curl -o ${SOURCE_DIR}/mac/PkgInfo.in "http://snaggledworks.com/software/wormux/PkgInfo.in";
	fi
else
	cd ${SOURCE_DIR};
	export OLD_SVN=`svn info | grep Revision | cut -d ' ' -f 2`;
	#svn up;
fi

export NEW_SVN=`svn info | grep Revision | cut -d ' ' -f 2`

if [ ${OLD_SVN} = ${NEW_SVN} ] ; then
	echo "The source has not changed since the last SVN build (revision ${OLD_SVN}).";
	echo "A new binary will not be built.";
#	exit 0;
fi

APP_VERSION=${APP_VERSION}-${NEW_SVN}

#./build_wormux_perso.sh

# configure the source

#./autogen.sh

echo ""
echo "Configuring the build settings using " ${CONFIGURE_PARAMS}
echo ""

#./configure ${CONFIGURE_PARAMS}

#/usr/bin/make -w clean
#/usr/bin/make -w

echo ""
echo "Done building the source"
echo "The .app bundle will now be created"
echo ""

# Build .app bundle
echo "Creating application bundle       ${APP_DIR}"
/bin/rm -rf ${APP_DIR}
/usr/bin/install -d -m 755 "${APP_DIR}"

# Contents 
echo "Creating Contents target          ${CNT_TARGET}"
/usr/bin/install -d -m 755 ${CNT_TARGET}

echo "Installing Info.plist."
/usr/bin/sed -e "s|%VERS%|${APP_VERSION}|g;s|%SERIAL%|${NEW_SVN}|g" mac/Info.plist.in > ${CNT_TARGET}/Info.plist

echo "Installing PkgInfo."
/usr/bin/install -m 644 mac/PkgInfo.in ${CNT_TARGET}/PkgInfo

echo "Creating MacOS target             ${MACOS_TARGET}"
/usr/bin/install -d -m 755 ${MACOS_TARGET}

echo "Copying executable from           ${SOURCE_DIR}/src"
/usr/bin/install -m 755 ${SOURCE_DIR}/src/${PROG_NAME} ${MACOS_TARGET}/${PROG_NAME}

echo "Stripping executable              ${PROG_NAME}"
/usr/bin/strip ${MACOS_TARGET}/${PROG_NAME}


# Copy the .dylibs that are linked by the program
echo "Copying the shared libraries to   ${MACOS_TARGET}"
for i in `${OTOOL} -L ${MACOS_TARGET}/${PROG_NAME} | grep version | cut -f 1 -d ' ' | grep -v \/System\/Library | grep -v \/usr\/lib` ; do
    ${CP} -p ${i} ${MACOS_TARGET};
    echo "Copied `basename ${i}`";
    ${INSTALL_NAME_TOOL} -change ${i} @executable_path/`basename ${i}` ${MACOS_TARGET}/${PROG_NAME};
    echo "Changed `basename ${i}` in ${PROG_NAME}";
    # now check paths for other libraries linked by the copied libraries.
    for j in `${OTOOL} -L ${i} | grep version | cut -f 1 -d ' '| grep -v ${i} | grep -v \/System\/Library | grep -v \/usr\/lib` ; do
        if [ ! -e ${MACOS_TARGET}/`basename ${j}` ] ; then
            ${CP} -p ${j} ${MACOS_TARGET};
            echo "  Copied `basename ${j}`";
            for k in `${OTOOL} -L ${j} | grep version | cut -f 1 -d ' '| grep -v ${j} | grep -v \/System\/Library | grep -v \/usr\/lib` ; do
                if [ ! -e ${MACOS_TARGET}/`basename ${k}` ] ; then
                    ${CP} -p ${k} ${MACOS_TARGET};
                    echo "    Copied `basename ${k}`";
                    for l in `${OTOOL} -L ${j} | grep version | cut -f 1 -d ' '| grep -v ${j} | grep -v \/System\/Library | grep -v \/usr\/lib` ; do
                        if [ ! -e ${MACOS_TARGET}/`basename ${l}` ] ; then
                            ${CP} -p ${l} ${MACOS_TARGET};
                            echo "      Copied `basename ${l}`";
                        fi
                    done
                fi
            done
        fi
    done
done

# Fix the install locations for the dylibs
echo "Modifying the linked libraries in ${MACOS_TARGET}"
for b in `${LS} -1 ${MACOS_TARGET}/*.dylib` ; do
    echo "Modifying `basename ${b}`"
    ${INSTALL_NAME_TOOL} -id @executable_path/`basename ${b}` ${MACOS_TARGET}/`basename ${b}`;
    for c in `otool -L ${b} | grep version | cut -f 1 -d ' ' | grep -v \`basename ${b}\` | grep -v \/System\/Library | grep -v \/usr\/lib` ; do
        ${INSTALL_NAME_TOOL} -change ${c} @executable_path/`basename ${c}` ${MACOS_TARGET}/`basename ${b}`;
        echo "  Changed `basename ${c}` in `basename ${b}`";
    done
done



# Create resources directory
echo "Creating Resources target         ${RSC_TARGET}"
/usr/bin/install -d -m 755 ${RSC_TARGET}

# Install data files
echo "Copying data files to             ${RSC_TARGET}/data"
/usr/bin/make -w -C ${SOURCE_DIR}/data -e install-nobase_dist_pkgdataDATA pkgdatadir=${RSC_TARGET}/data
#DESTDIR=${RSC_TARGET}/ pkgdatadir=data /usr/bin/make -w -C ${SOURCE_DIR}/data -e install

# Install the locale files
echo "Copying locale files to           ${RSC_TARGET}/locale"
/usr/bin/make -w -C ${SOURCE_DIR}/po -e all-local-yes DESTDIR=${RSC_TARGET}
/usr/bin/make -w -C ${SOURCE_DIR}/po -e install-data-local-yes localedir=${RSC_TARGET}/locale

# Copy .APP Icon
echo "Copying icon from                 ${SOURCE_DIR}/data"
/usr/bin/install -m 644 ${SOURCE_DIR}/data/wormux_128x128.icns ${RSC_TARGET}/${BUNDLE_NAME}.icns

echo ""
echo "The final ${BUNDLE_NAME}.app bundle is now complete"
echo ""

echo ""
echo "Creating the distributable disk image"
echo ""
DMG_TARGET="${BUNDLE_NAME} ${APP_VERSION}"

/bin/rm -f ${SOURCE_DIR}/${BUNDLE_NAME}-${APP_VERSION}-`uname -p`.dmg
/usr/bin/hdiutil create -type SPARSE -size 85m -fs HFS+ -volname "${DMG_TARGET}" -attach ${BUNDLE_NAME}-${APP_VERSION}.sparseimage
/bin/cp -R ${APP_DIR} "/Volumes/${DMG_TARGET}"

svn log -r ${OLD_SVN}:${NEW_SVN} > "/Volumes/${DMG_TARGET}/Changelog"

echo ""
echo "All files have been copied to the disk image /Volumes/${DMG_TARGET}"
echo ""

/usr/bin/hdiutil unmount "/Volumes/${DMG_TARGET}"
/usr/bin/hdiutil convert -imagekey zlib-level=9 -format UDZO ${BUNDLE_NAME}-${APP_VERSION}.sparseimage -o ${BUNDLE_NAME}-${APP_VERSION}-`uname -p`.dmg
/bin/rm -f ${BUNDLE_NAME}-${APP_VERSION}.sparseimage

echo ""
echo "Done building a distributable disk image for ${BUNDLE_NAME} ${APP_VERSION}."
echo "The disk image can be found at the root of the"
echo "build directory `pwd`"
echo "as '${BUNDLE_NAME}-${APP_VERSION}-`uname -p`.dmg'."

exit 0

#EOF

