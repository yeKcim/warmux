******************
Use of mac scripts
******************

First and foremost, the Xcode package must be installed in order to have gcc and other dev stuff.

The cmake_build.sh script builds Wormux on Mac OS X by using the CMake build system.
./cmake_build.sh --help will list its options. They currently are :

Requirements :
  - CMake 2.6 +
  - libintl (gnu gettext) headers (you can simply take the 'libintl.h' included along this
    script and copy it to /usr/local/include)
  - the mac dependencies package (http://download.gna.org/wormux/mac/mac_dependencies.zip)
    installed in your /Library/Frameworks directory (NOT $(HOME)/Library/Frameworks)
  - xgettext, part of gnu gettext (this dependency could probably be removed by tweaking CMake
    scripts to make them not update translation files, but for now it does)

targets :
    ./cmake_build.sh           : default build (i386, compatible OS X 10.5+)
    ./cmake_build.sh universal : build a universal app
    ./cmake_build.sh ppc       : cross-compile to PPC

arguments :
    -j<x>                      : launch make with x threads
    
To build a universal binary :
    1) build once in intel mode
    2) Save the generated .app
    3) Remove all build files
    4) Run again in PPC mode
    5) Merge both builds by using terminal app "lipo"
       (the frameworks from the dependency package are already universal)
    


Have fun :)


-- Auria, Plorf (Yoann Katchourine), Snaggle, lynxlynxlynx

