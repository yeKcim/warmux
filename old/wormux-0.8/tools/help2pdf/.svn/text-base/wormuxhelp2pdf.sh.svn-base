#!/bin/sh

print_usage() {
    echo "Usage: $0 lang"
    exit 1
}

check_binaries() {
    bins="wget xulrunner awk ps2pdf"
    for bin in $bins; do
	echo "looking for $bin..."
	which $bin
	if [ $? -ne 0 ]; then
	    echo "ERROR: you need to install $bin"
	    exit
	fi
    done
}

if [ "$1" = "" ]; then
    print_usage
fi

check_binaries

lang=$1
page=www.wormux.org/wiki/howto/$lang/play

toolsdir=`dirname $0`
outdir=`pwd`/wormux-doc

# create the output directory
mkdir -p $outdir

# creating a temporary directory
tmpdir=`mktemp -d /tmp/wormux.XXXXXX`

# downloading the page
echo "* downloading the page"

wget -q -p -k http://${page}.php -P ${tmpdir}
if [ $? -ne 0 ]; then
	echo "Doc for language ${lang} does not exist: failed to download ${page}"
	exit 2
fi

# removing headers
echo "* removing headers"
mv ${tmpdir}/${page}.php ${tmpdir}/${page}_all.html || exit 3
awk -f ${toolsdir}/getdoc.awk ${tmpdir}/${page}_all.html > ${tmpdir}/${page}.html || exit 4

# temporaly installing mozilla2ps
echo "* installing mozilla2ps in a temporary directory"
xulrunner --install-app ${toolsdir}/mozilla2ps-*.xulapp ${tmpdir}/ || exit 5

# exporting to ps using mozilla2ps (output path must be absolute path)
echo "* exporting to .ps"
html2ps="xulrunner ${tmpdir}/mozilla2ps/application.ini file://${tmpdir}/${page}.html ${outdir}/${lang}.ps"
#echo $html2ps
$html2ps || exit 6

if [ -f ${outdir}/${lang}.ps ]; then
    echo "File created: ${outdir}/${lang}.ps"
else
    echo "ERROR: file .ps has not been generated"
    exit 1
fi

# convert ps to pdf
echo "* converting to .pdf"
ps2pdf ${outdir}/${lang}.ps ${outdir}/${lang}.pdf

if [ -f ${outdir}/${lang}.pdf ]; then
    echo "File created: ${outdir}/${lang}.pdf"
else
    echo "ERROR: file .pdf has not been generated"
fi

# removing temp directory
rm -rf ${tmpdir}
