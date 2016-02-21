#!/bin/bash
# Quick and dirty script to check methods prototype in parent class
# Use check_src.sh first then retrieve_parent_class.sh.
# Authors : Drayan (yannigDOTperreATgmailDOTcom)
# 

# retrieve all parent of a given class
get_class_parent()
{
  current_class="$1"
  reverse=${2:-"false"}
  parent=$(egrep -e "^$current_class;" diagramme.def | awk -F\; '{ print $2 }')
  if [ ! -z "$parent" ]; then
    if [ "$reverse" = "true" ]; then
      echo "$(get_class_parent "$parent") $current_class"
    else
      echo "$current_class $(get_class_parent "$parent")"
    fi
  else
    echo "$current_class"
  fi
}

# return a clean C++ prototype without variable
get_raw_prototype()
{
  result="$1"
  for filter in 's/ *const */const/g' \
's/[ ]*&[ ]*/ & /g' \
's/[ ]*\*[ ]*/ * /g' \
's/ [A-Za-z_-]*)/)/g' \
's/=[ ]*0[ ]*//g' \
's/,[ ]*/, /g' \
's/(/( /g' \
's/)/ )/g' \
's/ *[a-zA-Z_-]*,/,/g' \
's/  */ /g' \
's/ ;/;/g'
  do
    result=$(echo "$result" | sed "$filter")
  done
  echo "$result" | tr -d '\15\32'
}

# return a basic prototype without const and public/private/protected
get_basic_prototype()
{
  result=$(get_raw_prototype "$1")
  for filter in 's/[ ]*const[ ]*/ /g' 's/[ ]*virtual[ ]*//g' 's/  */ /g' \
's/ [ ]*;/;/g'
  do
    result=$(echo "$result" | sed "$filter")
  done
  echo "$result" | sed 's/^[a-z]*://'
}

look_for_close_prototype()
{
  class="$1"
  prototype="$2"
  son_class="$3"
  basic_prot="$(get_basic_prototype "$prototype")"
  echo " >->-> looking for $prototype ($basic_prot)"
  no_prob="0"
  cat $class.classdef | while read line && [ $no_prob = "0" ]
  do
    cur_basic_prot="$(get_basic_prototype "$line")"
    if [ "$cur_basic_prot" = "$basic_prot" ]; then
      echo "  >->->-> Found same prototype in Class $class"
      raw_prot="$(get_raw_prototype "$line")"
      if [ "$prototype" = "$raw_prot" ]; then
	echo "  >->->-> Same prototype -> continue ($prototype)"
      else
        virtual_prototype="$(echo "$prototype" | sed 's/:/:virtual /')"
	if [ "$virtual_prototype" = "$raw_prot" ]; then
	  echo " Warning : forgot virtual keyword"
	  echo "  expected = $son_class/$raw_prot"
	  echo "  in class = $son_class/$prototype"
	else
          echo "  <->-<-> Prototype are different ! <->-<->"
	  echo "  EXPECTED = $son_class/$raw_prot"
	  echo "  IN CLASS = $son_class/$prototype"
	fi
        no_prob=1
      fi
    fi
  done
  return $no_prob
}

retrieve_class_parent()
{
  i=0
  for class_fic in *.classdef
  do
    classname=$(echo "$class_fic" | awk -F\. '{ print $1 }')
    cat $classname.classdef | tr -d '\15\32' | while read line
    do
      raw_prot="$(get_raw_prototype "$line")"
      if [ "$classname" = "$raw_prot" ]; then continue ; fi
      if echo "$raw_prot" | grep "$classname(" > /dev/null; then 
	echo "Constructor/Destructor : ignoring $raw_prot"
	continue
      fi      
      echo " > $classname : $raw_prot"
      for parent_class in $(get_class_parent "$classname" | \
                            sed "s/$classname//")
      do
	echo " >-> Entering $parent_class"
	look_for_close_prototype "$parent_class" "$(get_raw_prototype \
"$line")" "$classname"
	if [ $? = 1 ]; then
	  exit 1
	fi
	echo " <-< Leaving  $parent_class"
      done
      echo " < $classname : $(get_raw_prototype "$line")"
      echo
    done
    let i=i+1
    # if [ $i -gt 1 ]; then break; fi
    echo
  done
}

retrieve_class_parent
