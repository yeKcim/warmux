#!/bin/sh
last=$(ls *.r???? | awk -F\. '{ print $3 }' | sort -u -r | head -1)
for fic in *.$last
do
  mv $fic $(echo $fic | sed "s/.$last//")
done

rm *.r????
