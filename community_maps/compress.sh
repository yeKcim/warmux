#!/bin/sh

# check there is no case-sensitive file
check_case_files() {
    casefiles=`find ./ -name '*[A-Z]*' | grep -v '.svn' | grep -v 'README' | grep -v 'Makefile'`

    if [ "$casefiles" = "" ]; then
	return 0
    fi

    nbcasefiles=`/bin/echo -e "$casefiles" | wc -l`

    if [ $nbcasefiles -ne 0 ]; then
	/bin/echo -e "***********************************************************"
	/bin/echo -e "ERROR: there are $nbcasefiles case sensitive files. Please "
	/bin/echo -e "       rename them and check config.xml files"
	/bin/echo -e "$casefiles"
	/bin/echo -e "***********************************************************"
	return 1
    fi

    return 0
}

# check there is no space in filenames!
check_space_files() {
    spacefiles=`find ./ -name '* *' | grep -v '.svn' | grep -v 'README' | grep -v 'Makefile'`

    if [ "$spacefiles" = "" ]; then
	return 0
    fi

    nbspacefiles=`/bin/echo -e "$spacefiles" | wc -l`

    if [ $nbspacefiles -ne 0 ]; then
	/bin/echo -e "***********************************************************"
	/bin/echo -e "ERROR: there are $nbspacefiles files with space in their names."
	/bin/echo -e "Please rename them and check config.xml files"
	/bin/echo -e "$spacefiles"
	/bin/echo -e "***********************************************************"
	return 1
    fi

    return 0
}

check_files_exist() {
    filenames=`grep ' file=' $1/config.xml | sed 's/.*file=\"\(.*\)".*/\1/'`
    for f in $filenames; do
	if [ ! -f $1/$f ]; then
	    /bin/echo -e "*** ERROR: file $1/$f does not exist"
	    return 1
	fi
    done
}

check_water_type() {
    VALID_WATER_TYPE="no chocolate dirtywater lava radioactive water wine"
    water=`grep '<water>' $1/config.xml | sed 's%.*<water>\(.*\)</water>.*%\1%'`
    for w in $VALID_WATER_TYPE; do
	if [ "$water" = "$w" ]; then
	    return 0
	fi
    done

    echo "*** ERROR: Invalid water type $water for map $1"
    return 1
}

check_one_map_config() {
    check_files_exist $1

    if [ $? -ne 0 ]; then
	return 1
    fi

    check_water_type $1
}

check_maps_config() {
    for d in *; do
	if [ -d "$d" ]; then
	    check_one_map_config "$d";
	    if [ $? -ne 0 ]; then
		/bin/echo -e "*** ERROR: invalid configuration file for map $d"
		return 1
	    fi
	fi
    done
}

check_space_files || exit 1

check_case_files || exit 1

check_maps_config || exit 1

DATE=`date +%Y%m%d`
tar -czf WarMUX-CommunituMaps-${DATE}.tar.gz * --exclude=.svn --exclude=src --exclude=compress.sh --exclude=*~
