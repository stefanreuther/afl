##
##  Configuration section
##

use Cwd ('abs_path');

load_module ('Configure.pl');

# Variables
add_variable(RUN => '',
             PERL => $^X,
             WITH_COVERAGE => 0,
             COVERAGE_RESULT => "coverage_report",
             RM => 'rm -f',
             LDFLAGS => '',
             LIBDEPEND => '');

# Programs
find_compiler();
find_compiler_options("-I$V{IN}", "-I$V{OUT}", qw(-g -O2 -ansi -pedantic -fmessage-length=0 -Wno-long-long -Wconversion -mthreads -W -Wall));
find_archiver();

# Target
add_variable(TARGET => 'POSIX');
if ($V{TARGET} =~ /POSIX/i) {
    set_variable(CXXFLAGS => "$V{CXXFLAGS} -DTARGET_OS_POSIX");
    add_variable(EXE_SUFFIX => '');
    find_system_libraries(qw(-lpthread -lrt));
} elsif ($V{TARGET} =~ /Win(32|64)/i) {
    set_variable(CXXFLAGS => "$V{CXXFLAGS} -DTARGET_OS_WIN32");
    add_variable(EXE_SUFFIX => '.exe');
    find_system_libraries(qw(-lws2_32));
} else {
    die "Error: the specified target '$V{TARGET}' is not known; provide correct 'TARGET=' option";
}

# Libraries
find_library('WITH_OPENSSL',
             libs => '-lssl -lcrypto',
             program => "#include \"openssl/ssl.h\"\nint main() { TLSv1_client_method(); }\n",
             name => 'OpenSSL',
             dir => add_variable(OPENSSL_DIR => ''));
find_library('WITH_SCHANNEL',
             libs => '-lcrypt32',
             program => ("#include <windows.h>\n".
                         "#include <schannel.h>\n".
                         "#include <security.h>\n".
                         "#include <wincrypt.h>\n".
                         "SCHANNEL_CRED SchannelCred;\n".
                         "int main() { CertGetIssuerCertificateFromStore(0, 0, 0, 0); }\n"),
             name => 'SChannel (Win32 SSL)');
find_library('WITH_ZLIB',
             libs => '-lz',
             program => "#include <zlib.h>\nint main() { z_stream zs; inflateInit2(&zs, -15); }\n",
             name => "zlib",
             pkg => "zlib",
             dir => add_variable(ZLIB_DIR => ''));

# Generate output
mkdir "afl", 0777;
file_update('afl/config.h',
            ("#define HAVE_OPENSSL 1\n"  x $V{WITH_OPENSSL} ) .
            ("#define HAVE_ZLIB 1\n"     x $V{WITH_ZLIB}    ) .
            ("#define HAVE_SCHANNEL 1\n" x $V{WITH_SCHANNEL}));
file_update('config.mk',
            join ('', map {"CONFIG_AFL_$_ = $V{$_}\n"} sort keys %V));

# Options for coverage
if ($V{WITH_COVERAGE}) {
    set_variable(CXXFLAGS => "$V{CXXFLAGS} -O0 -fprofile-arcs -ftest-coverage");
    set_variable(LDFLAGS  => "$V{LDFLAGS} -fprofile-arcs -ftest-coverage");
}



##
##  Build section
##

load_module('Compiler.pl');

my $IN = $V{IN};
my $settings = load_variables("$IN/P9/Settings");

# Library
my $libafl = compile_static_library('afl', [to_prefix_list($IN, $settings->{FILES_afl})]);
generate('all', $libafl);

# Testsuite
{
    my @ts_src = sort(to_prefix_list($IN, $settings->{FILES_testsuite}));
    my @ts_obj = map {compile_file($_)} @ts_src;
    my $exe = compile_executable('testsuite', [@ts_obj], ['afl']);
    generate('test', 'testsuite', "$V{RUN} ./testsuite -p");
    rule_add_info('test', 'Running testsuite');
    rule_set_phony('test');

    # Coverage
    if ($V{WITH_COVERAGE}) {
        # Empty coverage (baseline)
        my $base = generate_anonymous('.info', $exe,
                                      "lcov -c -d $V{TMP} -i -o \$@");

        # Test run
        my $result = generate_anonymous('.info', $exe,
                                        "lcov -z -d $V{TMP}",            # Reset counters (delete all *.gcda)
                                        "$V{RUN} ./$exe",                # Run testee
                                        "lcov -c -d $V{TMP} -o \$@");    # Capture coverage (*.gcda -> info)

        # Combine captured and baseline so we can see unreferenced lines
        my $combined = generate_anonymous('.info', [$base, $result], "lcov -a $base -a $result -o \$@");

        # Filter only source code
        my $abs_in = abs_path($V{IN});
        my $filtered = generate_anonymous('.info', [$combined], "lcov -e \$< '$abs_in/*' -o \$@");

        # Optimize
        my $script = normalize_filename($V{IN}, 'scripts/coverage_optimize.pl');
        my $optimized = generate_anonymous('.info', [$filtered, $script], "$V{PERL} $script <\$< >\$@");

        # Generate HTML
        my $rm = $V{RM};
        my $report = $V{COVERAGE_RESULT};
        generate("$report/index.html", $optimized,
                 "$rm -r $result/*",              # "/*" to keep the .mark file intact which buildsys creates for us
                 "genhtml -t afl -o $report \$<");
        generate("coverage", "$report/index.html");
        rule_set_phony('coverage');
    }
}

# Utilities
my @utils;
foreach (split /\s+/, $settings->{TARGETS}) {
    my $ty = $settings->{"TYPE_$_"} || '';
    if ($ty eq 'app' && $_ ne 'testsuite') {
        push @utils, compile_executable($_, [to_prefix_list($IN, $settings->{"FILES_$_"})], ['afl']);
    }
}

##
##  Distribution section
##

# Publish everything
my $result_dir = get_variable('prefix');
foreach my $f (grep {/^afl.*hpp/} split /\s+/, $settings->{FILES_afl}) {
    generate('install', generate_copy("$result_dir/include/$f", "$V{IN}/$f"));
}
foreach my $f ($libafl) {
    generate('install', generate_copy("$result_dir/lib/$f", "$f"));
}
foreach my $f (@utils) {
    my ($dir, $base, $ext) = split_filename($f);
    generate('install', generate_copy_strip("$result_dir/bin/$base$ext", $f));
}

# We need to publish TestController.h despite not being an "official" API because it is convenient and c2ng uses it.
# Same thing for the coverage_optimize.pl script.
generate('install', generate_copy("$result_dir/bin/coverage_optimize.pl", "$IN/scripts/coverage_optimize.pl"));

# Publish configuration for benefit of consumers
generate('install', generate_copy("$result_dir/config.mk", "config.mk"));

rule_set_phony('install');

##
##  Additional Stuff
##

my $tagfile = add_variable(TAGFILE => 'TAGS');
generate('tags', [], "(cd $IN && etags -f - --recurse afl arch config u) > $tagfile");
rule_set_phony('tags');


sub generate_copy_strip {
    my ($out, $in) = @_;
    my $strip = add_variable(STRIP => 'strip');
    generate(generate_copy($out, $in),
             [],
             "$strip \$@");
}
