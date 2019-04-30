set -e

dir=$(dirname $0)/../.build-mingw64
cross=x86_64-w64-mingw32-
libdir=$(dirname $(${cross}gcc --print-libgcc-file-name))

mkdir -p $dir
cd $dir

perl ../../bs/Make.pl IN=.. RUN="WINEPATH=$libdir wine64" TARGET=Win64 CROSS_COMPILE=$cross
echo "Build files in $(pwd)"
