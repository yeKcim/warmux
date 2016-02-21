#!/bin/bash
# Quick and dirty script to retrieve methods prototype in parent class
# Use check_src.sh first then retrieve_parent_class.sh.
# Authors : Drayan (yannigDOTperreATgmailDOTcom)
# 

rm -f *.classdef diagramme.def
current_visibility="public"

retrieve_class_info()
{
  current_line="$*"
  classname=$(echo $current_line | awk '{ print $2 }')
  extended_class=$(echo $current_line | awk '{ print $5 }')
  if [ ! -z $extended_class ]; then
    echo "$header;Class $classname (extend $extended_class)"
    echo "$classname;$extended_class" >> diagramme.def
  else
    echo "$header;Class $classname"
    echo "$classname;" >> diagramme.def
  fi
  state="class"
}

clean_prototype()
{
  result="$*"
  for filter in 's|//.*||' 's|{.*}||' 's/ *;.*/;/' 's/ *( */( /'
  do
    result="$(echo "$result" | sed "$filter")"
  done
  echo "$result"
}

check_directory_headers()
{
  dir=$1
  for header in $(ls $dir/*.h)
  do
    state="header"
    cat $header | while read line
    do
      if echo $line | egrep -e "(private|protected|public):" > /dev/null; then
	current_visibility=$(echo "$line" | awk -F\: '{ print $1}')
      fi
      case $state in
        "header")
	  if echo "$line" | egrep -e ";|\*|#"> /dev/null; then continue; fi
          if echo "$line" | egrep -e "^class " > /dev/null; then
	    retrieve_class_info "$line"
	  fi
	  echo "$classname" >> "$classname.classdef"
          ;;
        "class")
          if echo "$line" | egrep -e "^class " > /dev/null; then
	    retrieve_class_info "$line"
	  fi
	  if echo "$line" | egrep ".*\(.*\).*;" > /dev/null; then
	    echo "$current_visibility:$(clean_prototype "$line")" >> \
		"$classname.classdef"
	  fi
          ;;
      esac
      sleep 0
    done
    echo
  done
}

for rep in object weapon
do
  echo "checking $rep"
  check_directory_headers $rep
  echo "done." ; echo
done
