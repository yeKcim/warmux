#!/bin/bash
DEBUG=${DEBUG:-"false"}
if $DEBUG; then set -x; fi
# A little script to install every needed package to compile wormux.
# Author : Yannig PERRE (drayan)
# Creation : 2007/10/20
# Modification : 2007/10/29 (drayan) : handling of urpmi/mandriva package
#

debian=true
mdv=false
fedora=false

case `uname -r` in
  *mdv) mdv=true ; debian=false ;;
esac

if $debian; then
  pkg_manager='apt-get install'
  pkg_lst='package.lst'
fi

if $mdv; then
  pkg_manager='urpmi'
  pkg_lst='mandriva.lst'
fi

if $fedora; then
  pkg_manager='yum install'
  pkg_lst='fedora.lst'
fi

if ! id | grep root > /dev/null; then
  echo
  echo "ERROR : You must be logged as root."
  echo
  echo "If you are running a Ubuntu/debian distribution, you should try to launch"
  echo "this script using 'sudo' this way :"
  echo
  echo "  sudo ./pkg_install.sh"
  echo
  exit
fi

echo
echo "The aim of this shell is to install a bunch of package in order to"
echo "compile the dev version of Wormux."
echo
echo "List of needed packages :"
echo

cat $pkg_lst

echo
echo -n "Do you want to install this list of package ?[y/n]"
read a

case $a in
  y|Y) : ;;
  *) echo "Exiting ..." ; exit ;;
esac

echo "Installing packages ..."
for pkg in $(cat $pkg_lst)
do
  $pkg_manager $pkg
done

