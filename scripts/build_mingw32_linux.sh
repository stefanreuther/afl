set -e

dir=$(dirname $0)/../.build-mingw32
cross=i686-w64-mingw32-
libdir=$(dirname $(${cross}gcc --print-libgcc-file-name))

mkdir -p $dir
cd $dir

perl ../../bs/Make.pl IN=.. RUN="WINEPATH=$libdir wine32" TARGET=Win32 CROSS_COMPILE=$cross
echo "Build files in $(pwd)"
