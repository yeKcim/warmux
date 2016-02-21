#!/bin/sh

print_usage() {
    echo "Usage: $0 lang"
    exit 1
}

if [ "$1" = "" ]; then
    print_usage
fi

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
mv ${tmpdir}/${page}.php ${tmpdir}/${page}_all.html
awk -f ${toolsdir}/getdoc.awk ${tmpdir}/${page}_all.html > ${tmpdir}/${page}.html

# temporaly installing mozilla2ps
xulrunner --install-app ${toolsdir}/mozilla2ps-*.xulapp ${tmpdir}/

# exporting to ps using mozilla2ps (output path must be absolute path)
echo "* exporting to .ps"
html2ps="xulrunner ${tmpdir}/mozilla2ps/application.ini file://${tmpdir}/${page}.html ${outdir}/${lang}.ps"
#echo $html2ps
$html2ps

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
