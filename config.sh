#!/bin/sh
#
#  Make a "config.mk" file
#
#  A trivial low-fat "configure" replacement.
#  Creates a config.mk (for make) and config.h (for the compiler).
#
#  Options:
#     VAR=VALUE         Set 'make' variable (instead of guessing it).
#                       Variables: PERL LDFLAGS LIBS CXX CXXFLAGS CXXTESTDIR
#     --with-PACKAGE    Assume package is present (instead of guessing)
#     --without-PACKAGE Assume package is not present and build without it (instead of guessing)
#                       Packages: openssl, zlib
#
#  If a package is forced present, the respective variables must also be forced completely:
#     ./config.sh --with-zlib LIBS="-lpthread -lrt -lz"
#

# Handling provided variables
needVariable() {
  var="$1"; shift
  eval $var=\"\"
  vars="$vars $var"
  for i; do
    if test "${i%%=*}" = "$var"; then
      echo "Using $i from command line."
      val="${i#*=}"
      eval $var=\$val
      return 1
    fi
  done
  return 0
}

# Handling provided packages
needPackage() {
  package="$1"; shift
  symbol="$1"; shift
  option=$(echo "$symbol" | tr A-Z_ a-z-)
  for i; do
    if test ":$i" = ":--with-$option"; then
      echo "Assuming $package already configured."
      addSymbol HAVE_$symbol
      return 1
    fi
    if test ":$i" = ":--without-$option"; then
      echo "Assuming $package not present."
      return 1
    fi
  done
  return 0
}

# Add a preprocessor symbol
addSymbol() {
  defines="$defines $@"
}

# Compile
compile() {
  echo > __tmp$$.cpp
  while test -n "$1" && test ":$1" != "::"; do
    echo "$1" >> __tmp$$.cpp; shift
  done
  if test ":$1" = "::"; then shift; fi
  result=false
  case "$1" in
    -[Ll]*) $CXX $CXXFLAGS __tmp$$.cpp "$@" -o __tmp$$ 2>/dev/null >/dev/null && result=true ;;
    *)      $CXX $CXXFLAGS "$@" __tmp$$.cpp -o __tmp$$ 2>/dev/null >/dev/null && result=true ;;
  esac
  rm -f __tmp$$.cpp
  rm -f __tmp$$
  $result
}

# Initial parameters
vars=""
defines=""


#
#  Actual Application Configuration:
#

# Trivial variables
if needVariable PERL "$@"; then
  PERL=perl
fi
needVariable LDFLAGS "$@"

# Find compiler
if needVariable CXX "$@"; then
  echo -n "Looking for compiler..."
  for i in g++-3.3 g++ c++; do
    CXX=$i
    if compile 'int main() { }' : -c; then 
      echo " $CXX"
      break
    fi
    CXX=""
  done
  if test -z "$CXX"; then
    echo " NONE FOUND."
    echo "   Use 'CXX=xxx' to define a compiler."
    exit 1
  fi
fi

# Find command line options
if needVariable CXXFLAGS "$@"; then
  echo -n "Looking for command line options..."
  result=""
  for i in -O2 -W -Wall -ansi -pedantic -fmessage-length=0 -Wno-long-long -Wconversion; do
    CXXFLAGS="$result $i"
    if compile 'int main() { }'; then
      result="$CXXFLAGS"
    fi
  done
  printf '%s\n' "$CXXFLAGS"
fi

# Find CxxTest
if needVariable CXXTESTDIR "$@"; then
  echo -n "Looking for CxxTest..."
  for i in ../cxxtest* ../../cxxtest* ../../../cxxtest*  ../software/cxxtest* ../../software/cxxtest* ../../../software/cxxtest*; do
    if test -f "$i/cxxtest/TestSuite.h"; then
      CXXTESTDIR=$i
      echo " $i"
      break
    fi
  done
  if test -z "$CXXTESTDIR"; then
    echo " NOT FOUND. You'll not be able to build tests."
    echo "   Use 'CXXTESTDIR=xxx' to define the directory."
  fi
fi

# Libraries
if needVariable LIBS "$@"; then
  echo -n "Looking for libraries..."
  for i in -lpthread -lrt -lws2_32; do
    if compile 'int main() { }' : $i; then
      LIBS="$LIBS $i"
    fi
  done
  printf '%s\n' "$LIBS"
fi

# Find OpenSSL
if needPackage OpenSSL OPENSSL "$@"; then
  echo -n "Looking for OpenSSL..."
  if compile '#include "openssl/ssl.h"' 'int main() { TLSv1_client_method(); }' : -lssl -lcrypto; then
    echo " found."
    addSymbol HAVE_OPENSSL
    LIBS="$LIBS -lssl -lcrypto"
  else
    echo " not found."
    echo "   Use LDFLAGS= and/or CXXFLAGS= to configure the compiler."
  fi
fi

# Find schannel
if needPackage SChannel SCHANNEL "$@"; then
  echo -n "Looking for SChannel (Win32 SSL)..."
  if compile '#include <windows.h>' '#include <schannel.h>' '#include <security.h>' '#include <wincrypt.h>' 'SCHANNEL_CRED SchannelCred;' \
      'int main() { CertGetIssuerCertificateFromStore(0, 0, 0, 0); }' : -lcrypt32; then
    echo " found."
    addSymbol HAVE_SCHANNEL
    LIBS="$LIBS -lcrypt32"
  else
    echo " not found."
    echo "   Use LDFLAGS= and/or CXXFLAGS= to configure the compiler."
  fi
fi

# Find zlib
if needPackage zlib ZLIB "$@"; then
  echo -n "Looking for zlib..."
  if compile '#include <zlib.h>' 'int main() { z_stream zs; inflateInit2(&zs, -15); }' : -lz; then
    echo " found."
    addSymbol HAVE_ZLIB
    LIBS="$LIBS -lz"
  else
    echo " not found."
    echo "   Use LDFLAGS= and/or CXXFLAGS= to configure the compiler."
  fi
fi

# Output
for i in $vars; do
  eval echo CONFIG_AFL_\$i = \$$i
done > config.mk
for i in $defines; do
  echo "#define $i 1"
done > afl/config.h
