#!/bin/sh

# WARNING: It runs only with xulrunner 1.8 ...
# http://releases.mozilla.org/pub/mozilla.org/xulrunner/releases/

print_usage() {
    echo "Usage: $0 lang lang2 lang3 ..."
    exit 1
}

if [ "$1" = "" ]; then
    print_usage
fi

generate_doc() {

    echo "** Generating documentation for lang $lang"

    lang=$1
    page=www.wormux.org/wiki/howto/$lang/play
    outdir=`pwd`/wormux-doc

    # create the output directory
    mkdir -p $outdir

    # creating a temporary directory
    tmpdir=`mktemp -d /tmp/wormux.XXXXXX`

    # downloading the page
    echo "* downloading the page"

    wget -q -p -k http://${page}.php -P ${tmpdir}
    if [ $? -ne 0 ]; then
	echo "ERROR: Doc for language ${lang} does not exist: failed to download ${page}"
	exit 2
    fi

    # removing headers
    echo "* removing headers"
    mv ${tmpdir}/${page}.php ${tmpdir}/${page}_all.html
    awk -f `dirname $0`/getdoc.awk ${tmpdir}/${page}_all.html > ${tmpdir}/${page}.html

    if [ $? -ne 0 ]; then
	echo "ERROR: Fail to remove headers!"
	exit 3
    fi

    # temporaly installing mozilla2ps
    echo "* installing mozilla2ps"
    ./xulrunner/xulrunner --install-app ./mozilla2ps-*.xulapp ${tmpdir}/

    if [ $? -ne 0 ]; then
	echo "ERROR: Fail to install mozilla2ps"
	exit 3
    fi


    # exporting to ps using mozilla2ps (output path must be absolute path)
    echo "* exporting to .ps"
    ./xulrunner/xulrunner ${tmpdir}/mozilla2ps/application.ini file://${tmpdir}/${page}.html ${outdir}/${lang}.ps

    if [ $? -ne 0 ]; then
	echo "ERROR: Fail to convert to .ps"
	exit 4
    fi

    if [ -f ${outdir}/${lang}.ps ]; then
	echo "File created: ${outdir}/${lang}.ps"
    else
	echo "ERROR file ${outdir}/${lang}.ps does not exist"
    fi

    # convert ps to pdf
    echo "* converting to .pdf"
    ps2pdf ${outdir}/${lang}.ps ${outdir}/${lang}.pdf

    if [ $? -ne 0 ]; then
	echo "ERROR: Fail to convert to .pdf"
	exit 4
    fi

    if [ -f ${outdir}/${lang}.pdf ]; then
	echo "File created: ${outdir}/${lang}.pdf"
    else
	echo "ERROR file ${outdir}/${lang}.pdf does not exist"
    fi

    # removing temp directory
    rm -rf ${tmpdir}

    echo ""
}

for lang in $@; do
    generate_doc $lang
done
