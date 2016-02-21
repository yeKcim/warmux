#!/bin/sh
# This script searches for files, cmake needs to know about
# data files for installation and source files for compilation ;)

cd `dirname $0`

# Search data files that should be installed
echo "
SET(DATA_FILES
`find ../data/ -type f | grep -v -e "\.svn\|Makefile\|~" | cut -c 9- | sort`
)" > InstallData.cmake

