set -e

dir=$(dirname $0)/../.build-native

mkdir -p $dir
cd $dir

perl ../../bs/Make.pl IN=..
echo "Build files in $(pwd)"
