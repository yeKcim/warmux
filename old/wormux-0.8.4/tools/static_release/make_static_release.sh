#!/bin/bash

HERE="`pwd`"
WORMUX_DIR=$HERE/wormux

ARCHIVE_NAME="wormux-clanlibstatic-"
## Version number based on the one from wormux/configure.ac,
## is added at the end of the archive name.

FILES="AUTHORS CHANGELOG.txt COPYING HACKERS.txt README wormux \
	BUGS config.xml FAQ.txt INSTALL po TODO.txt locale/ doc/ \
	data/game_mode/ data/font/ data/map/araignee/ data/map/arbre/ \
	data/map/banquise/ data/map/cowland/ data/map/grenouilles/ \
	data/map/halloween/ data/map/libellule/ data/map/mer/ \
	data/map/noel/ data/map/pirates/ data/map/prehistorik/ \
	data/map/space/ data/skin/alien/ data/skin/ecureuil/ \
	data/skin/escargot/ data/skin/flament_rose/ data/skin/poulpe/ \
	data/team/alien/ data/team/escargot/ data/team/nuts/ \
	data/team/pink_floyd/ data/team/poulpe/ data/gfx/ data/interface/ \
	data/menu/ data/sky/ data/object/ data/sound/ data/weapon/ \
	data/graphism.xml data/sound.xml data/weapons.xml"

# # get from the cvs
echo "* Update to the current version from CVS"
cvs -d:pserver:anonymous@cvs.gna.org:/cvs/wormux co wormux ||exit

# # Normal compilation
echo "* Do the standard compilation"
cd $WORMUX_DIR
./autogen.sh && ./configure && make || exit

# Semi-static compilation
echo "* Make the semi-static compilation"
cp $HERE/static.sh $WORMUX_DIR/static.sh
chmod 755 $WORMUX_DIR/static.sh
./static.sh || exit

# replace with the good INSTALL file
cd $HERE
echo "* Replace INSTALL file"
rm -rf $WORMUX_DIR/INSTALL*
cp INSTALL $WORMUX_DIR/INSTALL
cp config.xml $WORMUX_DIR/config.xml

# retrieve version from configure.ac
VERSION=$(grep AC_INIT $WORMUX_DIR/configure.ac|awk 'BEGIN {FS="["}{print $3}'|sed -e "s/\], //")
ARCHIVE_NAME="$ARCHIVE_NAME$VERSION"
ln -s wormux wormux-$VERSION

# make an archive
echo "* Make an archive"
tar chf "$ARCHIVE_NAME.tar" wormux-$VERSION/wormux

# Add all files to the archive
for folders in $FILES
do
	for file in $(find wormux-$VERSION/$folders|grep -v /$|grep -v CVS|grep -v ~$|grep -v Makefile|grep -v \.po$)
	do
		# Don't add folders!!
		if [ ! -d $file ];then
			echo Adding file $file ...
			tar uhf "$ARCHIVE_NAME.tar" $file
		fi
	done
done

rm wormux-$VERSION

echo 
echo "* Bzipping the archive"
bzip2 "$ARCHIVE_NAME.tar"
echo "* The archive $ARCHIVE_NAME.tar.bz2 is ready"
