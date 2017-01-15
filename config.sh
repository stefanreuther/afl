#!/bin/bash
#
#  Configuration for afl
#
#  Variables to generate:
#    CXXTESTDIR
#    PERL
#    CXX, CXXFLAGS
#    LIBS, LDFLAGS, [LIBDEPEND]
#    AR
#    RUN
#
#  Macros to generate:
#    HAVE_ZLIB
#    HAVE_OPENSSL
#

conf_user_help() {
    echo "Packages:
  openssl
  schannel
  zlib

Variables:
  AR
  CXX
  CXXFLAGS
  CXXTESTDIR
  LDFLAGS
  LIBDEPEND
  LIBS
  PERL
  RUN
"
}

. ./config.lib

##
##  Programs
##

# Find compiler
conf_log_start "Looking for C++ compiler..."
conf_compile_set_program "int main() { }"
if test -n "$CXX"; then
    conf_compile_try CXX=$CXX || conf_log_die "The configured compiler, \"$CXX\", does not work." >&2
else
    for i in ${CROSS_COMPILE}g++ ${CROSS_COMPILE}c++; do
        conf_compile_try CXX=$i && break
    done
    test -z "$CXX" && conf_log_die "No C++ compiler."
fi
conf_log_result "$CXX"

# Find compiler options
conf_log_start "Looking for C++ compiler flags..."
conf_var_set CXXFLAGS "$CXXFLAGS"
for i in -g -O2 -ansi -pedantic -fmessage-length=0 -Wno-long-long -Wconversion -mthreads -W -Wall; do
    # build it backwards so user options are at the end and can override ours
    conf_compile_try CXXFLAGS="$i $CXXFLAGS"
done
conf_log_result "$CXXFLAGS"

# Find archiver
conf_log_start "Looking for archiver..."
conf_var_set AR "${AR:-ar}"
conf_run $AR cr __tmp.a || conf_log_die "Does not work."
rm -f __tmp.a
conf_log_result "$AR"

# Set RUN variable (no guess)
conf_var_publish RUN

# Find perl
conf_log_start "Checking for perl..."
conf_var_set PERL "${PERL:-perl}"
conf_run $PERL -e 'use 5;' || conf_log_die "Does not work."
conf_log_result "$PERL"

##
##  Libraries
##

conf_var_publish LIBS LDFLAGS LIBDEPEND

# CxxTest
conf_log_start "Looking for CxxTest..."
conf_file_set cxxtest/TestSuite.h cxxtestgen.pl
if test -z "$CXXTESTDIR"; then
    for i in $CXXTESTDIR ../cxxtest* ../../cxxtest* ../../../cxxtest*  ../software/cxxtest* ../../software/cxxtest* ../../../software/cxxtest*; do
        if conf_file_check_dir "$i"; then
            conf_var_set CXXTESTDIR "$i"
            conf_log_result "$CXXTESTDIR"
            break
        fi
    done
    if test -z "$CXXTESTDIR"; then
        conf_log_result "not found"
        conf_var_set CXXTESTDIR ""
    fi
else
    conf_file_check_dir "$CXXTESTDIR" || conf_log_die "The specified directory \"$CXXTESTDIR\" does not work."
    conf_log_result "OK"
    conf_var_publish CXXTESTDIR
fi

# Libraries
conf_log_start "Looking for libraries..."
conf_compile_set_program "int main() { }"
if test -z "$LIBS"; then
    for i in -lpthread -lrt -lws2_32
    do
        conf_link_try LIBS="$LIBS $i"
    done
else
    conf_link_try || conf_log_die "The configured library list, \"$LIBS\", does not work."
fi
conf_log_result "$LIBS"

# OpenSSL
conf_log_start "Looking for OpenSSL..."
conf_compile_set_program "
  #include \"openssl/ssl.h\"
  int main() { TLSv1_client_method(); }"
conf_link_try_syslib openssl LIBS="$LIBS -lssl -lcrypto" &&
    conf_macro_set HAVE_OPENSSL


# SChannel
conf_log_start "Looking for SChannel (Win32 SSL)..."
conf_compile_set_program "
  #include <windows.h>
  #include <schannel.h>
  #include <security.h>
  #include <wincrypt.h>
  SCHANNEL_CRED SchannelCred;
  int main() { CertGetIssuerCertificateFromStore(0, 0, 0, 0); }"
conf_link_try_syslib schannel LIBS="$LIBS -lcrypt32" &&
    conf_macro_set HAVE_SCHANNEL

# zlib
conf_log_start "Looking for zlib..."
conf_compile_set_program "
  #include <zlib.h>
  int main() { z_stream zs; inflateInit2(&zs, -15); }"
if conf_link_try_syslib zlib || conf_link_try_syslib zlib LIBS="$LIBS -lz"; then
    conf_macro_set HAVE_ZLIB
fi


##
##  Generate output
##

conf_var_write_makefile CONFIG_AFL_ | conf_update_file config.mk
conf_macro_write_header             | conf_update_file afl/config.h
conf_cleanup
