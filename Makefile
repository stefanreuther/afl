#
# Makefile
#

ALL_TARGETS = libafl.a date dialog ls chatserver wget secureio respclient \
    respserver httpserver env unzip testsuite
OBJECTS_afl = afl/net/server.o afl/sys/commandlineparser.o \
    afl/except/commandlineexception.o arch/posix/posixdialog.o \
    arch/win32/win32dialog.o afl/sys/dialog.o afl/base/baseweaklink.o \
    afl/base/weaktarget.o afl/io/transformreaderstream.o \
    afl/io/deflatetransform.o afl/io/archive/zipreader.o \
    afl/io/limitedstream.o afl/io/msexpandtransform.o \
    afl/io/nullfilesystem.o afl/string/posixfilenames.o \
    afl/net/parameterencoder.o afl/charset/unicode.o afl/io/nulltextwriter.o \
    afl/io/textwriter.o afl/io/textreader.o \
    afl/except/assertionfailedexception.o afl/sys/longcommandlineparser.o \
    afl/string/proxytranslator.o afl/net/http/simpledownloadlistener.o \
    afl/sys/standardcommandlineparser.o afl/io/datasink.o \
    afl/data/namequery.o arch/win32/win32filemapping.o \
    arch/posix/posixfilemapping.o arch/posix/posixenvironment.o \
    arch/win32/win32environment.o afl/io/internalfilemapping.o \
    afl/charset/defaultcharsetfactory.o afl/io/xml/defaultentityhandler.o \
    afl/io/xml/reader.o afl/net/http/formparser.o afl/io/textsink.o \
    afl/net/http/pageresponse.o afl/net/http/pagerequest.o \
    afl/net/http/pagedispatcher.o afl/net/http/errorresponse.o \
    afl/net/http/request.o afl/net/http/protocolhandler.o \
    afl/net/commandhandler.o afl/net/resp/protocolhandler1.o \
    afl/net/redis/internaldatabase.o afl/data/segmentview.o \
    afl/net/redis/stringsetoperation.o afl/net/redis/integersetoperation.o \
    afl/net/redis/stringsetkey.o afl/net/redis/integersetkey.o \
    afl/net/redis/setkey.o afl/net/redis/sortoperation.o \
    afl/net/redis/stringlistkey.o afl/net/redis/integerlistkey.o \
    afl/net/redis/listkey.o afl/net/redis/subtree.o \
    afl/net/redis/stringfield.o afl/net/redis/integerfield.o \
    afl/net/redis/field.o afl/net/redis/hashkey.o afl/net/redis/stringkey.o \
    afl/net/redis/integerkey.o afl/net/redis/key.o \
    afl/async/communicationsink.o afl/net/resp/client1.o \
    afl/io/internalsink.o afl/io/bufferedsink.o afl/data/errorvalue.o \
    afl/io/transformdatasink.o afl/io/inflatetransform.o \
    arch/win32/win32root.o arch/win32/win32directory.o \
    arch/win32/win32stream.o arch/win32/win32filesystem.o afl/sys/mutex1.o \
    afl/sys/semaphore1.o afl/sys/thread.o afl/base/deleter.o \
    afl/container/ptrmultilistbase.o afl/base/signalhandler.o \
    afl/base/signalconnection.o afl/sys/atomicinteger.o afl/base/ptr.o \
    afl/bits/bits.o afl/checksums/bytesum.o afl/checksums/crc32.o \
    afl/checksums/crc16.o afl/checksums/adler32.o afl/string/string.o \
    afl/string/format.o afl/string/formatstate.o afl/string/formattraits.o \
    afl/charset/utf8reader.o afl/charset/utf8.o afl/sys/error.o \
    arch/posix/posix.o arch/win32/win32.o afl/except/fileproblemexception.o \
    afl/except/fileformatexception.o afl/except/filetooshortexception.o \
    afl/except/filesystemexception.o afl/except/systemexception.o \
    afl/io/stream.o afl/io/filesystem.o arch/posix/posixfilesystem.o \
    arch/posix/posixstream.o afl/io/nullstream.o afl/io/directoryentry.o \
    afl/io/directory.o arch/posix/posixdirectory.o arch/posix/posixroot.o \
    afl/io/multidirectory.o afl/io/memorystream.o afl/io/constmemorystream.o \
    afl/io/internalstream.o afl/io/multiplexablestream.o \
    afl/io/internaldirectory.o afl/string/translator.o \
    afl/string/nulltranslator.o afl/charset/utf8charset.o \
    afl/charset/codepage_1250.o afl/charset/codepage_1251.o \
    afl/charset/codepage_1252.o afl/charset/codepage_437.o \
    afl/charset/codepage_850.o afl/charset/codepage_852.o \
    afl/charset/codepage_866.o afl/charset/codepage_koi8r.o \
    afl/charset/codepage_latin1.o afl/charset/codepage_latin2.o \
    afl/charset/codepagecharset.o afl/io/textfile.o afl/io/bufferedstream.o \
    afl/net/name.o afl/net/networkstack.o arch/posix/posixnetworkstack.o \
    afl/bits/fixedstring.o arch/win32/win32networkstack.o \
    afl/charset/quotedprintable.o afl/charset/urlencoding.o \
    afl/charset/base64.o afl/sys/time.o arch/posix/posixtime.o \
    afl/sys/duration.o arch/win32/win32time.o afl/async/controller.o \
    afl/async/semaphore.o afl/async/operation.o afl/async/mutex.o \
    afl/async/messageexchange.o afl/async/timer.o \
    arch/posix/posixcontrollerimpl.o afl/async/receiveoperation.o \
    arch/posix/posixfiledescriptor.o afl/sys/environment.o \
    arch/win32/win32controllerimpl.o afl/async/communicationstream.o \
    afl/data/namemap.o afl/data/integervalue.o afl/data/booleanvalue.o \
    afl/data/stringvalue.o afl/data/visitor.o afl/data/floatvalue.o \
    afl/data/segment.o afl/data/hash1.o afl/data/hashvalue.o \
    afl/data/vector.o afl/data/vectorvalue.o afl/data/defaultvaluefactory.o \
    afl/io/json/parser.o afl/io/json/writer.o afl/string/parse.o \
    afl/data/access.o afl/except/remoteerrorexception.o \
    afl/except/invaliddataexception.o afl/io/resp/writer1.o \
    afl/io/resp/parser1.o afl/net/url.o afl/net/simpleserver.o \
    afl/net/headerparser.o afl/async/notifier.o afl/net/http/client.o \
    afl/net/http/clientresponse.o afl/string/parsememory.o \
    afl/net/http/clientconnection.o afl/net/http/chunkedsink.o \
    afl/io/limiteddatasink.o afl/except/unsupportedexception.o \
    afl/io/inflatedatasink.o afl/sys/log.o afl/net/headerfield.o \
    afl/net/headertable.o afl/net/http/defaultconnectionprovider.o \
    afl/net/http/manager.o afl/net/http/cookie.o afl/net/http/cookiejar.o \
    afl/sys/loglistener.o afl/net/securesocket.o \
    config/openssl/opensslexception.o config/openssl/opensslsocket.o \
    afl/net/securenetworkstack.o config/openssl/opensslcontext.o \
    afl/net/securecontext.o afl/checksums/md5.o afl/checksums/hash.o \
    afl/checksums/sha1.o
AR = $(CONFIG_AFL_AR)
OBJECTS_chatserver = app/chatserver.o
LIBDEPEND = $(CONFIG_AFL_LIBDEPEND)
CXX = $(CONFIG_AFL_CXX)
LDFLAGS = $(CONFIG_AFL_LDFLAGS)
LIBS = $(CONFIG_AFL_LIBS)
OBJECTS_date = app/date.o
OBJECTS_dialog = app/dialog1.o
OBJECTS_env = app/env.o
OBJECTS_httpserver = app/httpserver.o
OBJECTS_ls = app/ls.o
OBJECTS_respclient = app/respclient.o
OBJECTS_respserver = app/respserver.o
OBJECTS_secureio = app/secureio.o
OBJECTS_testsuite = u/t_net_server.o u/t_base_observable.o u/t_sys_dialog.o \
    u/t_base_weaklink.o u/t_base_baseweaklink.o u/t_base_weaktarget.o \
    u/t_io_transformreaderstream.o u/t_io_json_parser_testsuite.o \
    u/t_io_deflatetransform.o u/t_io_archive_zipreader.o \
    u/t_io_limitedstream.o u/t_base_ref.o u/t_net_http_errorresponse.o \
    u/t_io_msexpandtransform.o u/t_net_redis_sortoperation.o \
    u/t_net_redis_stringsetkey.o u/t_net_redis_integersetkey.o \
    u/t_net_redis_subtree.o u/t_net_redis_key.o u/t_net_redis_stringkey.o \
    u/mock/commandhandlermock.o u/t_net_redis_integerkey.o \
    u/t_io_nullfilesystem.o u/t_string_posixfilenames.o \
    u/t_net_http_client.o u/t_net_parameterencoder.o u/t_io_nulltextwriter.o \
    u/t_io_textreader.o u/t_io_textwriter.o \
    u/t_except_assertionfailedexception.o u/t_sys_longcommandlineparser.o \
    u/t_string_proxytranslator.o u/t_sys_standardcommandlineparser.o \
    u/t_sys_commandlineparser.o u/t_data_namequery.o \
    u/t_base_inlineoptional.o u/t_base_clonable.o u/t_base_growablememory.o \
    u/t_functional_stringtable.o u/t_functional_mapping.o \
    u/t_functional_binaryfunction.o u/t_functional_unaryfunction.o \
    u/t_io_internalfilemapping.o u/t_io_filemapping.o \
    u/t_charset_defaultcharsetfactory.o u/t_charset_charsetfactory.o \
    u/t_io_xml_defaultentityhandler.o u/t_io_xml_entityhandler.o \
    u/t_io_xml_basereader.o u/t_io_xml_reader.o u/t_net_http_formparser.o \
    u/t_io_textsink.o u/t_net_http_pagedispatcher.o u/t_net_http_page.o \
    u/t_net_http_pageresponse.o u/t_net_http_pagerequest.o \
    u/t_net_http_response.o u/t_net_http_dispatcher.o u/t_net_http_request.o \
    u/t_net_commandhandler.o u/t_net_resp_protocolhandler.o \
    u/t_container_ptrqueue.o u/t_net_redis_internaldatabase.o \
    u/t_data_segmentview.o u/t_net_resp_client.o \
    u/t_async_communicationsink.o u/t_io_bufferedsink.o \
    u/t_io_internalsink.o u/t_data_stringlist.o u/t_data_integerlist.o \
    u/t_data_errorvalue.o u/t_io_transformdatasink.o \
    u/t_io_inflatetransform.o u/t_io_transform.o testsuite.o \
    u/t_base_memory.o u/t_container_ptrvector.o u/t_sys_mutex.o \
    u/t_sys_thread.o u/t_base_runnable.o u/t_base_deletable.o \
    u/t_sys_mutexguard.o u/t_sys_semaphore.o u/t_base_deleter.o \
    u/t_container_ptrmap.o u/t_base_enumerator.o \
    u/t_container_ptrmultilist.o u/t_container_ptrmultilistbase.o \
    u/t_tmp_ifthenelse.o u/t_base_staticassert.o u/t_base_closure.o \
    u/t_base_clonableref.o u/t_base_signalhandler.o u/t_base_countof.o \
    u/t_bits_smallset.o u/t_base_signalconnection.o u/t_base_signal.o \
    u/t_sys_atomicinteger.o u/t_base_refcounted.o u/t_base_ptr.o \
    u/t_bits_rotate.o u/t_bits_bits.o u/t_checksums_bytesum.o \
    u/t_checksums_crc32.o u/t_checksums_crc16.o u/t_checksums_adler32.o \
    u/t_tmp_stripcv.o u/t_string_char.o u/t_string_string.o \
    u/t_string_format.o u/t_string_formatstate.o u/t_tmp_sizedint.o \
    u/t_tmp_sizeduint.o u/t_tmp_issametype.o u/t_tmp_typelist.o \
    u/t_container_dereferencingiterator.o u/t_container_memberiterator.o \
    u/t_tmp_dereferencetype.o u/t_charset_utf8reader.o u/t_tmp_copycv.o \
    u/t_charset_utf8.o u/t_charset_unicode.o u/t_bits_pack.o \
    u/t_bits_uint8.o u/t_bits_uint16le.o u/t_bits_uint32le.o \
    u/t_bits_uint64le.o u/t_bits_int16le.o u/t_bits_int32le.o \
    u/t_bits_int8.o u/t_bits_int64le.o u/t_sys_error.o u/t_io_stream.o \
    u/t_except.o u/t_io_filesystem.o u/t_io_nullstream.o \
    u/t_io_directoryentry.o u/t_io_directory.o u/t_io_memorystream.o \
    u/t_io_constmemorystream.o u/t_io_internalstream.o \
    u/t_io_multiplexablestream.o u/t_io_internaldirectory.o \
    u/t_string_translator.o u/t_string_nulltranslator.o \
    u/t_string_messages.o u/t_bits_value.o u/t_charset_charset.o \
    u/t_charset_utf8charset.o u/t_charset_codepage.o \
    u/t_charset_codepagecharset.o u/t_io_textfile.o u/t_io_bufferedstream.o \
    u/t_net_name.o u/t_net_socket.o u/t_net_networkstack.o \
    u/t_net_listener.o u/t_bits_fixedstring.o u/t_charset_quotedprintable.o \
    u/t_charset_urlencoding.o u/t_charset_base64.o u/t_sys_duration.o \
    u/t_sys_time.o u/t_async_semaphore.o u/t_async_mutex.o \
    u/t_async_messageexchange.o u/t_async_timer.o u/t_async_sendoperation.o \
    u/t_async_receiveoperation.o u/t_async_cancelable.o \
    u/t_async_communicationobject.o u/t_async_synchronisationobject.o \
    u/t_async_controller.o u/t_async_operation.o u/t_async_operationlist.o \
    u/t_sys_environment.o u/t_async_communicationstream.o u/t_data_namemap.o \
    u/t_data_value.o u/t_data_scalarvalue.o u/t_data_integervalue.o \
    u/t_data_booleanvalue.o u/t_data_floatvalue.o u/t_data_stringvalue.o \
    u/t_data_segment.o u/t_data_hash.o u/t_data_hashvalue.o \
    u/t_data_vector.o u/t_data_vectorvalue.o u/t_data_valuefactory.o \
    u/t_data_defaultvaluefactory.o u/t_io_json_parser.o u/t_io_json_writer.o \
    u/t_string_parse.o u/t_data_access.o u/t_io_resp_writer.o \
    u/t_io_resp_parser.o u/t_net_url.o u/t_net_acceptoperation.o \
    u/t_net_protocolhandler.o u/t_net_protocolhandlerfactory.o \
    u/t_net_simpleserver.o u/t_net_headerconsumer.o u/t_net_headerparser.o \
    u/t_async_notifier.o u/t_string_parsememory.o \
    u/t_net_http_clientresponse.o u/t_net_http_clientconnectionprovider.o \
    u/t_net_http_clientrequest.o u/t_net_http_clientconnection.o \
    u/t_io_datasink.o u/t_net_http_chunkedsink.o u/t_io_limiteddatasink.o \
    u/t_io_inflatedatasink.o u/t_sys_log.o u/t_net_headerfield.o \
    u/t_net_headertable.o u/t_net_http_defaultconnectionprovider.o \
    u/t_net_http_downloadlistener.o u/t_net_http_cookie.o \
    u/t_net_http_cookiejar.o u/t_sys_loglistener.o u/t_base_types.o \
    u/t_base_uncopyable.o u/t_checksums_checksum.o u/t_data_visitor.o \
    u/t_string_formatunion.o u/t_string_formattraits.o u/t_sys_types.o \
    u/t_sys_parsedtime.o u/t_net_securesocket.o u/t_net_securecontext.o \
    u/t_net_securenetworkstack.o u/t_io_multidirectory.o u/t_checksums_md5.o \
    u/t_checksums_hash.o u/t_checksums_sha1.o u/t_bits_int16be.o \
    u/t_bits_int32be.o u/t_bits_int64be.o u/t_bits_uint16be.o \
    u/t_bits_uint32be.o u/t_bits_uint64be.o u/t_base_optional.o
OBJECTS_unzip = app/unzip.o
OBJECTS_wget = app/wget.o
CXXFLAGS = $(CONFIG_AFL_CXXFLAGS) -I. -DTARGET_OS_POSIX -MMD
RM = rm
RUN = $(CONFIG_AFL_RUN)
HEADERS_testsuite = u/t_*.hpp
PERL = $(CONFIG_AFL_PERL)
CXXTESTDIR = $(CONFIG_AFL_CXXTESTDIR)

include config.mk

.PHONY: clean distclean all afl

.SUFFIXES: .cpp .lo .o .s

all: $(ALL_TARGETS)

libafl.a: $(OBJECTS_afl)
	@echo "        Archiving libafl.a..."
	@$(AR) cru libafl.a $(OBJECTS_afl)

chatserver: $(OBJECTS_chatserver) libafl.a $(LIBDEPEND)
	@echo "        Linking chatserver..."
	@$(CXX) $(LDFLAGS) -o chatserver $(OBJECTS_chatserver) -L. -lafl $(LIBS)

date: $(OBJECTS_date) libafl.a $(LIBDEPEND)
	@echo "        Linking date..."
	@$(CXX) $(LDFLAGS) -o date $(OBJECTS_date) -L. -lafl $(LIBS)

dialog: $(OBJECTS_dialog) libafl.a $(LIBDEPEND)
	@echo "        Linking dialog..."
	@$(CXX) $(LDFLAGS) -o dialog $(OBJECTS_dialog) -L. -lafl $(LIBS)

env: $(OBJECTS_env) libafl.a $(LIBDEPEND)
	@echo "        Linking env..."
	@$(CXX) $(LDFLAGS) -o env $(OBJECTS_env) -L. -lafl $(LIBS)

httpserver: $(OBJECTS_httpserver) libafl.a $(LIBDEPEND)
	@echo "        Linking httpserver..."
	@$(CXX) $(LDFLAGS) -o httpserver $(OBJECTS_httpserver) -L. -lafl $(LIBS)

ls: $(OBJECTS_ls) libafl.a $(LIBDEPEND)
	@echo "        Linking ls..."
	@$(CXX) $(LDFLAGS) -o ls $(OBJECTS_ls) -L. -lafl $(LIBS)

respclient: $(OBJECTS_respclient) libafl.a $(LIBDEPEND)
	@echo "        Linking respclient..."
	@$(CXX) $(LDFLAGS) -o respclient $(OBJECTS_respclient) -L. -lafl $(LIBS)

respserver: $(OBJECTS_respserver) libafl.a $(LIBDEPEND)
	@echo "        Linking respserver..."
	@$(CXX) $(LDFLAGS) -o respserver $(OBJECTS_respserver) -L. -lafl $(LIBS)

secureio: $(OBJECTS_secureio) libafl.a $(LIBDEPEND)
	@echo "        Linking secureio..."
	@$(CXX) $(LDFLAGS) -o secureio $(OBJECTS_secureio) -L. -lafl $(LIBS)

testsuite: $(OBJECTS_testsuite) libafl.a $(LIBDEPEND)
	@echo "        Linking testsuite..."
	@$(CXX) $(LDFLAGS) -o testsuite $(OBJECTS_testsuite) -L. -lafl $(LIBS)

unzip: $(OBJECTS_unzip) libafl.a $(LIBDEPEND)
	@echo "        Linking unzip..."
	@$(CXX) $(LDFLAGS) -o unzip $(OBJECTS_unzip) -L. -lafl $(LIBS)

wget: $(OBJECTS_wget) libafl.a $(LIBDEPEND)
	@echo "        Linking wget..."
	@$(CXX) $(LDFLAGS) -o wget $(OBJECTS_wget) -L. -lafl $(LIBS)

Makefile: P9/Settings P9/Rules-unix.mak P9/Settings-unix.mak
	@echo "        Regenerating Makefile..."
	@proj9 update

afl: libafl.a

afl/data/hash1.lo: afl/data/hash.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/data/hash1.lo -c afl/data/hash.cpp

afl/data/hash1.o: afl/data/hash.cpp
	@echo "        Compiling afl/data/hash.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/data/hash1.o -c afl/data/hash.cpp

afl/data/hash1.s: afl/data/hash.cpp
	$(CXX) $(CXXFLAGS) -o afl/data/hash1.s -S afl/data/hash.cpp

afl/io/resp/parser1.lo: afl/io/resp/parser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/io/resp/parser1.lo -c afl/io/resp/parser.cpp

afl/io/resp/parser1.o: afl/io/resp/parser.cpp
	@echo "        Compiling afl/io/resp/parser.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/io/resp/parser1.o -c afl/io/resp/parser.cpp

afl/io/resp/parser1.s: afl/io/resp/parser.cpp
	$(CXX) $(CXXFLAGS) -o afl/io/resp/parser1.s -S afl/io/resp/parser.cpp

afl/io/resp/writer1.lo: afl/io/resp/writer.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/io/resp/writer1.lo -c afl/io/resp/writer.cpp

afl/io/resp/writer1.o: afl/io/resp/writer.cpp
	@echo "        Compiling afl/io/resp/writer.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/io/resp/writer1.o -c afl/io/resp/writer.cpp

afl/io/resp/writer1.s: afl/io/resp/writer.cpp
	$(CXX) $(CXXFLAGS) -o afl/io/resp/writer1.s -S afl/io/resp/writer.cpp

afl/net/resp/client1.lo: afl/net/resp/client.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/net/resp/client1.lo -c afl/net/resp/client.cpp

afl/net/resp/client1.o: afl/net/resp/client.cpp
	@echo "        Compiling afl/net/resp/client.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/net/resp/client1.o -c afl/net/resp/client.cpp

afl/net/resp/client1.s: afl/net/resp/client.cpp
	$(CXX) $(CXXFLAGS) -o afl/net/resp/client1.s -S afl/net/resp/client.cpp

afl/net/resp/protocolhandler1.lo: afl/net/resp/protocolhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/net/resp/protocolhandler1.lo -c afl/net/resp/protocolhandler.cpp

afl/net/resp/protocolhandler1.o: afl/net/resp/protocolhandler.cpp
	@echo "        Compiling afl/net/resp/protocolhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/net/resp/protocolhandler1.o -c afl/net/resp/protocolhandler.cpp

afl/net/resp/protocolhandler1.s: afl/net/resp/protocolhandler.cpp
	$(CXX) $(CXXFLAGS) -o afl/net/resp/protocolhandler1.s -S afl/net/resp/protocolhandler.cpp

afl/sys/mutex1.lo: afl/sys/mutex.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/sys/mutex1.lo -c afl/sys/mutex.cpp

afl/sys/mutex1.o: afl/sys/mutex.cpp
	@echo "        Compiling afl/sys/mutex.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/sys/mutex1.o -c afl/sys/mutex.cpp

afl/sys/mutex1.s: afl/sys/mutex.cpp
	$(CXX) $(CXXFLAGS) -o afl/sys/mutex1.s -S afl/sys/mutex.cpp

afl/sys/semaphore1.lo: afl/sys/semaphore.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o afl/sys/semaphore1.lo -c afl/sys/semaphore.cpp

afl/sys/semaphore1.o: afl/sys/semaphore.cpp
	@echo "        Compiling afl/sys/semaphore.cpp..."
	@$(CXX) $(CXXFLAGS) -o afl/sys/semaphore1.o -c afl/sys/semaphore.cpp

afl/sys/semaphore1.s: afl/sys/semaphore.cpp
	$(CXX) $(CXXFLAGS) -o afl/sys/semaphore1.s -S afl/sys/semaphore.cpp

app/dialog1.lo: app/dialog.cpp
	$(CXX) -fPIC $(CXXFLAGS) -o app/dialog1.lo -c app/dialog.cpp

app/dialog1.o: app/dialog.cpp
	@echo "        Compiling app/dialog.cpp..."
	@$(CXX) $(CXXFLAGS) -o app/dialog1.o -c app/dialog.cpp

app/dialog1.s: app/dialog.cpp
	$(CXX) $(CXXFLAGS) -o app/dialog1.s -S app/dialog.cpp

clean:
	$(RM) $(OBJECTS_afl)
	$(RM) $(OBJECTS_date)
	$(RM) $(OBJECTS_dialog)
	$(RM) $(OBJECTS_ls)
	$(RM) $(OBJECTS_chatserver)
	$(RM) $(OBJECTS_wget)
	$(RM) $(OBJECTS_secureio)
	$(RM) $(OBJECTS_respclient)
	$(RM) $(OBJECTS_respserver)
	$(RM) $(OBJECTS_httpserver)
	$(RM) $(OBJECTS_env)
	$(RM) $(OBJECTS_unzip)
	$(RM) $(OBJECTS_testsuite)

config.mk afl/config.h:
	@echo "        Configuring..."
	@sh config.sh

depend.mk: Makefile
	@echo "        Regenerating depend.mk..."
	@for i in $(OBJECTS_afl) $(OBJECTS_date) $(OBJECTS_dialog) $(OBJECTS_ls) $(OBJECTS_chatserver) $(OBJECTS_wget) $(OBJECTS_secureio) $(OBJECTS_respclient) $(OBJECTS_respserver) $(OBJECTS_httpserver) $(OBJECTS_env) $(OBJECTS_unzip) $(OBJECTS_testsuite); do echo "-include $${i%o}d"; done > depend.mk

distclean: clean
	$(RM) testsuite.cpp
	$(RM) libafl.a
	$(RM) date
	$(RM) dialog
	$(RM) ls
	$(RM) chatserver
	$(RM) wget
	$(RM) secureio
	$(RM) respclient
	$(RM) respserver
	$(RM) httpserver
	$(RM) env
	$(RM) unzip
	$(RM) testsuite

test: testsuite
	@echo "        Running testsuite..."
	@$(RUN) ./testsuite

testsuite.cpp: $(HEADERS_testsuite)
	@echo "        Generating test driver..."
	@$(PERL) $(CXXTESTDIR)/cxxtestgen.pl --gui=TestController --have-eh --error-printer -o $@ $(HEADERS_testsuite)

testsuite.lo: testsuite.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -U_CXXTEST_HAVE_EH -U_CXXTEST_HAVE_STD -O0 -o testsuite.lo -c testsuite.cpp

testsuite.o: testsuite.cpp
	@echo "        Compiling testsuite.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -U_CXXTEST_HAVE_EH -U_CXXTEST_HAVE_STD -O0 -o testsuite.o -c testsuite.cpp

testsuite.s: testsuite.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -U_CXXTEST_HAVE_EH -U_CXXTEST_HAVE_STD -O0 -o testsuite.s -S testsuite.cpp

u/mock/commandhandlermock.lo: u/mock/commandhandlermock.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/mock/commandhandlermock.lo -c u/mock/commandhandlermock.cpp

u/mock/commandhandlermock.o: u/mock/commandhandlermock.cpp
	@echo "        Compiling u/mock/commandhandlermock.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/mock/commandhandlermock.o -c u/mock/commandhandlermock.cpp

u/mock/commandhandlermock.s: u/mock/commandhandlermock.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/mock/commandhandlermock.s -S u/mock/commandhandlermock.cpp

u/t_async_cancelable.lo: u/t_async_cancelable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_cancelable.lo -c u/t_async_cancelable.cpp

u/t_async_cancelable.o: u/t_async_cancelable.cpp
	@echo "        Compiling u/t_async_cancelable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_cancelable.o -c u/t_async_cancelable.cpp

u/t_async_cancelable.s: u/t_async_cancelable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_cancelable.s -S u/t_async_cancelable.cpp

u/t_async_communicationobject.lo: u/t_async_communicationobject.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationobject.lo -c u/t_async_communicationobject.cpp

u/t_async_communicationobject.o: u/t_async_communicationobject.cpp
	@echo "        Compiling u/t_async_communicationobject.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationobject.o -c u/t_async_communicationobject.cpp

u/t_async_communicationobject.s: u/t_async_communicationobject.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationobject.s -S u/t_async_communicationobject.cpp

u/t_async_communicationsink.lo: u/t_async_communicationsink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationsink.lo -c u/t_async_communicationsink.cpp

u/t_async_communicationsink.o: u/t_async_communicationsink.cpp
	@echo "        Compiling u/t_async_communicationsink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationsink.o -c u/t_async_communicationsink.cpp

u/t_async_communicationsink.s: u/t_async_communicationsink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationsink.s -S u/t_async_communicationsink.cpp

u/t_async_communicationstream.lo: u/t_async_communicationstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationstream.lo -c u/t_async_communicationstream.cpp

u/t_async_communicationstream.o: u/t_async_communicationstream.cpp
	@echo "        Compiling u/t_async_communicationstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationstream.o -c u/t_async_communicationstream.cpp

u/t_async_communicationstream.s: u/t_async_communicationstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_communicationstream.s -S u/t_async_communicationstream.cpp

u/t_async_controller.lo: u/t_async_controller.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_controller.lo -c u/t_async_controller.cpp

u/t_async_controller.o: u/t_async_controller.cpp
	@echo "        Compiling u/t_async_controller.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_controller.o -c u/t_async_controller.cpp

u/t_async_controller.s: u/t_async_controller.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_controller.s -S u/t_async_controller.cpp

u/t_async_messageexchange.lo: u/t_async_messageexchange.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_messageexchange.lo -c u/t_async_messageexchange.cpp

u/t_async_messageexchange.o: u/t_async_messageexchange.cpp
	@echo "        Compiling u/t_async_messageexchange.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_messageexchange.o -c u/t_async_messageexchange.cpp

u/t_async_messageexchange.s: u/t_async_messageexchange.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_messageexchange.s -S u/t_async_messageexchange.cpp

u/t_async_mutex.lo: u/t_async_mutex.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_mutex.lo -c u/t_async_mutex.cpp

u/t_async_mutex.o: u/t_async_mutex.cpp
	@echo "        Compiling u/t_async_mutex.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_mutex.o -c u/t_async_mutex.cpp

u/t_async_mutex.s: u/t_async_mutex.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_mutex.s -S u/t_async_mutex.cpp

u/t_async_notifier.lo: u/t_async_notifier.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_notifier.lo -c u/t_async_notifier.cpp

u/t_async_notifier.o: u/t_async_notifier.cpp
	@echo "        Compiling u/t_async_notifier.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_notifier.o -c u/t_async_notifier.cpp

u/t_async_notifier.s: u/t_async_notifier.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_notifier.s -S u/t_async_notifier.cpp

u/t_async_operation.lo: u/t_async_operation.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operation.lo -c u/t_async_operation.cpp

u/t_async_operation.o: u/t_async_operation.cpp
	@echo "        Compiling u/t_async_operation.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operation.o -c u/t_async_operation.cpp

u/t_async_operation.s: u/t_async_operation.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operation.s -S u/t_async_operation.cpp

u/t_async_operationlist.lo: u/t_async_operationlist.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operationlist.lo -c u/t_async_operationlist.cpp

u/t_async_operationlist.o: u/t_async_operationlist.cpp
	@echo "        Compiling u/t_async_operationlist.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operationlist.o -c u/t_async_operationlist.cpp

u/t_async_operationlist.s: u/t_async_operationlist.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_operationlist.s -S u/t_async_operationlist.cpp

u/t_async_receiveoperation.lo: u/t_async_receiveoperation.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_receiveoperation.lo -c u/t_async_receiveoperation.cpp

u/t_async_receiveoperation.o: u/t_async_receiveoperation.cpp
	@echo "        Compiling u/t_async_receiveoperation.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_receiveoperation.o -c u/t_async_receiveoperation.cpp

u/t_async_receiveoperation.s: u/t_async_receiveoperation.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_receiveoperation.s -S u/t_async_receiveoperation.cpp

u/t_async_semaphore.lo: u/t_async_semaphore.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_semaphore.lo -c u/t_async_semaphore.cpp

u/t_async_semaphore.o: u/t_async_semaphore.cpp
	@echo "        Compiling u/t_async_semaphore.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_semaphore.o -c u/t_async_semaphore.cpp

u/t_async_semaphore.s: u/t_async_semaphore.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_semaphore.s -S u/t_async_semaphore.cpp

u/t_async_sendoperation.lo: u/t_async_sendoperation.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_sendoperation.lo -c u/t_async_sendoperation.cpp

u/t_async_sendoperation.o: u/t_async_sendoperation.cpp
	@echo "        Compiling u/t_async_sendoperation.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_sendoperation.o -c u/t_async_sendoperation.cpp

u/t_async_sendoperation.s: u/t_async_sendoperation.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_sendoperation.s -S u/t_async_sendoperation.cpp

u/t_async_synchronisationobject.lo: u/t_async_synchronisationobject.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_synchronisationobject.lo -c u/t_async_synchronisationobject.cpp

u/t_async_synchronisationobject.o: u/t_async_synchronisationobject.cpp
	@echo "        Compiling u/t_async_synchronisationobject.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_synchronisationobject.o -c u/t_async_synchronisationobject.cpp

u/t_async_synchronisationobject.s: u/t_async_synchronisationobject.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_synchronisationobject.s -S u/t_async_synchronisationobject.cpp

u/t_async_timer.lo: u/t_async_timer.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_timer.lo -c u/t_async_timer.cpp

u/t_async_timer.o: u/t_async_timer.cpp
	@echo "        Compiling u/t_async_timer.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_timer.o -c u/t_async_timer.cpp

u/t_async_timer.s: u/t_async_timer.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_async_timer.s -S u/t_async_timer.cpp

u/t_base_baseweaklink.lo: u/t_base_baseweaklink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_baseweaklink.lo -c u/t_base_baseweaklink.cpp

u/t_base_baseweaklink.o: u/t_base_baseweaklink.cpp
	@echo "        Compiling u/t_base_baseweaklink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_baseweaklink.o -c u/t_base_baseweaklink.cpp

u/t_base_baseweaklink.s: u/t_base_baseweaklink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_baseweaklink.s -S u/t_base_baseweaklink.cpp

u/t_base_clonable.lo: u/t_base_clonable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonable.lo -c u/t_base_clonable.cpp

u/t_base_clonable.o: u/t_base_clonable.cpp
	@echo "        Compiling u/t_base_clonable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonable.o -c u/t_base_clonable.cpp

u/t_base_clonable.s: u/t_base_clonable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonable.s -S u/t_base_clonable.cpp

u/t_base_clonableref.lo: u/t_base_clonableref.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonableref.lo -c u/t_base_clonableref.cpp

u/t_base_clonableref.o: u/t_base_clonableref.cpp
	@echo "        Compiling u/t_base_clonableref.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonableref.o -c u/t_base_clonableref.cpp

u/t_base_clonableref.s: u/t_base_clonableref.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_clonableref.s -S u/t_base_clonableref.cpp

u/t_base_closure.lo: u/t_base_closure.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_closure.lo -c u/t_base_closure.cpp

u/t_base_closure.o: u/t_base_closure.cpp
	@echo "        Compiling u/t_base_closure.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_closure.o -c u/t_base_closure.cpp

u/t_base_closure.s: u/t_base_closure.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_closure.s -S u/t_base_closure.cpp

u/t_base_countof.lo: u/t_base_countof.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_countof.lo -c u/t_base_countof.cpp

u/t_base_countof.o: u/t_base_countof.cpp
	@echo "        Compiling u/t_base_countof.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_countof.o -c u/t_base_countof.cpp

u/t_base_countof.s: u/t_base_countof.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_countof.s -S u/t_base_countof.cpp

u/t_base_deletable.lo: u/t_base_deletable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deletable.lo -c u/t_base_deletable.cpp

u/t_base_deletable.o: u/t_base_deletable.cpp
	@echo "        Compiling u/t_base_deletable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deletable.o -c u/t_base_deletable.cpp

u/t_base_deletable.s: u/t_base_deletable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deletable.s -S u/t_base_deletable.cpp

u/t_base_deleter.lo: u/t_base_deleter.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deleter.lo -c u/t_base_deleter.cpp

u/t_base_deleter.o: u/t_base_deleter.cpp
	@echo "        Compiling u/t_base_deleter.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deleter.o -c u/t_base_deleter.cpp

u/t_base_deleter.s: u/t_base_deleter.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_deleter.s -S u/t_base_deleter.cpp

u/t_base_enumerator.lo: u/t_base_enumerator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_enumerator.lo -c u/t_base_enumerator.cpp

u/t_base_enumerator.o: u/t_base_enumerator.cpp
	@echo "        Compiling u/t_base_enumerator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_enumerator.o -c u/t_base_enumerator.cpp

u/t_base_enumerator.s: u/t_base_enumerator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_enumerator.s -S u/t_base_enumerator.cpp

u/t_base_growablememory.lo: u/t_base_growablememory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_growablememory.lo -c u/t_base_growablememory.cpp

u/t_base_growablememory.o: u/t_base_growablememory.cpp
	@echo "        Compiling u/t_base_growablememory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_growablememory.o -c u/t_base_growablememory.cpp

u/t_base_growablememory.s: u/t_base_growablememory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_growablememory.s -S u/t_base_growablememory.cpp

u/t_base_inlineoptional.lo: u/t_base_inlineoptional.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_inlineoptional.lo -c u/t_base_inlineoptional.cpp

u/t_base_inlineoptional.o: u/t_base_inlineoptional.cpp
	@echo "        Compiling u/t_base_inlineoptional.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_inlineoptional.o -c u/t_base_inlineoptional.cpp

u/t_base_inlineoptional.s: u/t_base_inlineoptional.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_inlineoptional.s -S u/t_base_inlineoptional.cpp

u/t_base_memory.lo: u/t_base_memory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_memory.lo -c u/t_base_memory.cpp

u/t_base_memory.o: u/t_base_memory.cpp
	@echo "        Compiling u/t_base_memory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_memory.o -c u/t_base_memory.cpp

u/t_base_memory.s: u/t_base_memory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_memory.s -S u/t_base_memory.cpp

u/t_base_observable.lo: u/t_base_observable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_observable.lo -c u/t_base_observable.cpp

u/t_base_observable.o: u/t_base_observable.cpp
	@echo "        Compiling u/t_base_observable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_observable.o -c u/t_base_observable.cpp

u/t_base_observable.s: u/t_base_observable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_observable.s -S u/t_base_observable.cpp

u/t_base_optional.lo: u/t_base_optional.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_optional.lo -c u/t_base_optional.cpp

u/t_base_optional.o: u/t_base_optional.cpp
	@echo "        Compiling u/t_base_optional.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_optional.o -c u/t_base_optional.cpp

u/t_base_optional.s: u/t_base_optional.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_optional.s -S u/t_base_optional.cpp

u/t_base_ptr.lo: u/t_base_ptr.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ptr.lo -c u/t_base_ptr.cpp

u/t_base_ptr.o: u/t_base_ptr.cpp
	@echo "        Compiling u/t_base_ptr.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ptr.o -c u/t_base_ptr.cpp

u/t_base_ptr.s: u/t_base_ptr.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ptr.s -S u/t_base_ptr.cpp

u/t_base_ref.lo: u/t_base_ref.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ref.lo -c u/t_base_ref.cpp

u/t_base_ref.o: u/t_base_ref.cpp
	@echo "        Compiling u/t_base_ref.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ref.o -c u/t_base_ref.cpp

u/t_base_ref.s: u/t_base_ref.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_ref.s -S u/t_base_ref.cpp

u/t_base_refcounted.lo: u/t_base_refcounted.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_refcounted.lo -c u/t_base_refcounted.cpp

u/t_base_refcounted.o: u/t_base_refcounted.cpp
	@echo "        Compiling u/t_base_refcounted.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_refcounted.o -c u/t_base_refcounted.cpp

u/t_base_refcounted.s: u/t_base_refcounted.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_refcounted.s -S u/t_base_refcounted.cpp

u/t_base_runnable.lo: u/t_base_runnable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_runnable.lo -c u/t_base_runnable.cpp

u/t_base_runnable.o: u/t_base_runnable.cpp
	@echo "        Compiling u/t_base_runnable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_runnable.o -c u/t_base_runnable.cpp

u/t_base_runnable.s: u/t_base_runnable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_runnable.s -S u/t_base_runnable.cpp

u/t_base_signal.lo: u/t_base_signal.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signal.lo -c u/t_base_signal.cpp

u/t_base_signal.o: u/t_base_signal.cpp
	@echo "        Compiling u/t_base_signal.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signal.o -c u/t_base_signal.cpp

u/t_base_signal.s: u/t_base_signal.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signal.s -S u/t_base_signal.cpp

u/t_base_signalconnection.lo: u/t_base_signalconnection.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalconnection.lo -c u/t_base_signalconnection.cpp

u/t_base_signalconnection.o: u/t_base_signalconnection.cpp
	@echo "        Compiling u/t_base_signalconnection.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalconnection.o -c u/t_base_signalconnection.cpp

u/t_base_signalconnection.s: u/t_base_signalconnection.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalconnection.s -S u/t_base_signalconnection.cpp

u/t_base_signalhandler.lo: u/t_base_signalhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalhandler.lo -c u/t_base_signalhandler.cpp

u/t_base_signalhandler.o: u/t_base_signalhandler.cpp
	@echo "        Compiling u/t_base_signalhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalhandler.o -c u/t_base_signalhandler.cpp

u/t_base_signalhandler.s: u/t_base_signalhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_signalhandler.s -S u/t_base_signalhandler.cpp

u/t_base_staticassert.lo: u/t_base_staticassert.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_staticassert.lo -c u/t_base_staticassert.cpp

u/t_base_staticassert.o: u/t_base_staticassert.cpp
	@echo "        Compiling u/t_base_staticassert.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_staticassert.o -c u/t_base_staticassert.cpp

u/t_base_staticassert.s: u/t_base_staticassert.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_staticassert.s -S u/t_base_staticassert.cpp

u/t_base_types.lo: u/t_base_types.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_types.lo -c u/t_base_types.cpp

u/t_base_types.o: u/t_base_types.cpp
	@echo "        Compiling u/t_base_types.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_types.o -c u/t_base_types.cpp

u/t_base_types.s: u/t_base_types.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_types.s -S u/t_base_types.cpp

u/t_base_uncopyable.lo: u/t_base_uncopyable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_uncopyable.lo -c u/t_base_uncopyable.cpp

u/t_base_uncopyable.o: u/t_base_uncopyable.cpp
	@echo "        Compiling u/t_base_uncopyable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_uncopyable.o -c u/t_base_uncopyable.cpp

u/t_base_uncopyable.s: u/t_base_uncopyable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_uncopyable.s -S u/t_base_uncopyable.cpp

u/t_base_weaklink.lo: u/t_base_weaklink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaklink.lo -c u/t_base_weaklink.cpp

u/t_base_weaklink.o: u/t_base_weaklink.cpp
	@echo "        Compiling u/t_base_weaklink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaklink.o -c u/t_base_weaklink.cpp

u/t_base_weaklink.s: u/t_base_weaklink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaklink.s -S u/t_base_weaklink.cpp

u/t_base_weaktarget.lo: u/t_base_weaktarget.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaktarget.lo -c u/t_base_weaktarget.cpp

u/t_base_weaktarget.o: u/t_base_weaktarget.cpp
	@echo "        Compiling u/t_base_weaktarget.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaktarget.o -c u/t_base_weaktarget.cpp

u/t_base_weaktarget.s: u/t_base_weaktarget.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_base_weaktarget.s -S u/t_base_weaktarget.cpp

u/t_bits_bits.lo: u/t_bits_bits.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_bits.lo -c u/t_bits_bits.cpp

u/t_bits_bits.o: u/t_bits_bits.cpp
	@echo "        Compiling u/t_bits_bits.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_bits.o -c u/t_bits_bits.cpp

u/t_bits_bits.s: u/t_bits_bits.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_bits.s -S u/t_bits_bits.cpp

u/t_bits_fixedstring.lo: u/t_bits_fixedstring.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_fixedstring.lo -c u/t_bits_fixedstring.cpp

u/t_bits_fixedstring.o: u/t_bits_fixedstring.cpp
	@echo "        Compiling u/t_bits_fixedstring.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_fixedstring.o -c u/t_bits_fixedstring.cpp

u/t_bits_fixedstring.s: u/t_bits_fixedstring.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_fixedstring.s -S u/t_bits_fixedstring.cpp

u/t_bits_int16be.lo: u/t_bits_int16be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16be.lo -c u/t_bits_int16be.cpp

u/t_bits_int16be.o: u/t_bits_int16be.cpp
	@echo "        Compiling u/t_bits_int16be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16be.o -c u/t_bits_int16be.cpp

u/t_bits_int16be.s: u/t_bits_int16be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16be.s -S u/t_bits_int16be.cpp

u/t_bits_int16le.lo: u/t_bits_int16le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16le.lo -c u/t_bits_int16le.cpp

u/t_bits_int16le.o: u/t_bits_int16le.cpp
	@echo "        Compiling u/t_bits_int16le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16le.o -c u/t_bits_int16le.cpp

u/t_bits_int16le.s: u/t_bits_int16le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int16le.s -S u/t_bits_int16le.cpp

u/t_bits_int32be.lo: u/t_bits_int32be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32be.lo -c u/t_bits_int32be.cpp

u/t_bits_int32be.o: u/t_bits_int32be.cpp
	@echo "        Compiling u/t_bits_int32be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32be.o -c u/t_bits_int32be.cpp

u/t_bits_int32be.s: u/t_bits_int32be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32be.s -S u/t_bits_int32be.cpp

u/t_bits_int32le.lo: u/t_bits_int32le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32le.lo -c u/t_bits_int32le.cpp

u/t_bits_int32le.o: u/t_bits_int32le.cpp
	@echo "        Compiling u/t_bits_int32le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32le.o -c u/t_bits_int32le.cpp

u/t_bits_int32le.s: u/t_bits_int32le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int32le.s -S u/t_bits_int32le.cpp

u/t_bits_int64be.lo: u/t_bits_int64be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64be.lo -c u/t_bits_int64be.cpp

u/t_bits_int64be.o: u/t_bits_int64be.cpp
	@echo "        Compiling u/t_bits_int64be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64be.o -c u/t_bits_int64be.cpp

u/t_bits_int64be.s: u/t_bits_int64be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64be.s -S u/t_bits_int64be.cpp

u/t_bits_int64le.lo: u/t_bits_int64le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64le.lo -c u/t_bits_int64le.cpp

u/t_bits_int64le.o: u/t_bits_int64le.cpp
	@echo "        Compiling u/t_bits_int64le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64le.o -c u/t_bits_int64le.cpp

u/t_bits_int64le.s: u/t_bits_int64le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int64le.s -S u/t_bits_int64le.cpp

u/t_bits_int8.lo: u/t_bits_int8.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int8.lo -c u/t_bits_int8.cpp

u/t_bits_int8.o: u/t_bits_int8.cpp
	@echo "        Compiling u/t_bits_int8.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int8.o -c u/t_bits_int8.cpp

u/t_bits_int8.s: u/t_bits_int8.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_int8.s -S u/t_bits_int8.cpp

u/t_bits_pack.lo: u/t_bits_pack.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_pack.lo -c u/t_bits_pack.cpp

u/t_bits_pack.o: u/t_bits_pack.cpp
	@echo "        Compiling u/t_bits_pack.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_pack.o -c u/t_bits_pack.cpp

u/t_bits_pack.s: u/t_bits_pack.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_pack.s -S u/t_bits_pack.cpp

u/t_bits_rotate.lo: u/t_bits_rotate.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_rotate.lo -c u/t_bits_rotate.cpp

u/t_bits_rotate.o: u/t_bits_rotate.cpp
	@echo "        Compiling u/t_bits_rotate.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_rotate.o -c u/t_bits_rotate.cpp

u/t_bits_rotate.s: u/t_bits_rotate.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_rotate.s -S u/t_bits_rotate.cpp

u/t_bits_smallset.lo: u/t_bits_smallset.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_smallset.lo -c u/t_bits_smallset.cpp

u/t_bits_smallset.o: u/t_bits_smallset.cpp
	@echo "        Compiling u/t_bits_smallset.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_smallset.o -c u/t_bits_smallset.cpp

u/t_bits_smallset.s: u/t_bits_smallset.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_smallset.s -S u/t_bits_smallset.cpp

u/t_bits_uint16be.lo: u/t_bits_uint16be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16be.lo -c u/t_bits_uint16be.cpp

u/t_bits_uint16be.o: u/t_bits_uint16be.cpp
	@echo "        Compiling u/t_bits_uint16be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16be.o -c u/t_bits_uint16be.cpp

u/t_bits_uint16be.s: u/t_bits_uint16be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16be.s -S u/t_bits_uint16be.cpp

u/t_bits_uint16le.lo: u/t_bits_uint16le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16le.lo -c u/t_bits_uint16le.cpp

u/t_bits_uint16le.o: u/t_bits_uint16le.cpp
	@echo "        Compiling u/t_bits_uint16le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16le.o -c u/t_bits_uint16le.cpp

u/t_bits_uint16le.s: u/t_bits_uint16le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint16le.s -S u/t_bits_uint16le.cpp

u/t_bits_uint32be.lo: u/t_bits_uint32be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32be.lo -c u/t_bits_uint32be.cpp

u/t_bits_uint32be.o: u/t_bits_uint32be.cpp
	@echo "        Compiling u/t_bits_uint32be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32be.o -c u/t_bits_uint32be.cpp

u/t_bits_uint32be.s: u/t_bits_uint32be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32be.s -S u/t_bits_uint32be.cpp

u/t_bits_uint32le.lo: u/t_bits_uint32le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32le.lo -c u/t_bits_uint32le.cpp

u/t_bits_uint32le.o: u/t_bits_uint32le.cpp
	@echo "        Compiling u/t_bits_uint32le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32le.o -c u/t_bits_uint32le.cpp

u/t_bits_uint32le.s: u/t_bits_uint32le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint32le.s -S u/t_bits_uint32le.cpp

u/t_bits_uint64be.lo: u/t_bits_uint64be.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64be.lo -c u/t_bits_uint64be.cpp

u/t_bits_uint64be.o: u/t_bits_uint64be.cpp
	@echo "        Compiling u/t_bits_uint64be.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64be.o -c u/t_bits_uint64be.cpp

u/t_bits_uint64be.s: u/t_bits_uint64be.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64be.s -S u/t_bits_uint64be.cpp

u/t_bits_uint64le.lo: u/t_bits_uint64le.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64le.lo -c u/t_bits_uint64le.cpp

u/t_bits_uint64le.o: u/t_bits_uint64le.cpp
	@echo "        Compiling u/t_bits_uint64le.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64le.o -c u/t_bits_uint64le.cpp

u/t_bits_uint64le.s: u/t_bits_uint64le.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint64le.s -S u/t_bits_uint64le.cpp

u/t_bits_uint8.lo: u/t_bits_uint8.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint8.lo -c u/t_bits_uint8.cpp

u/t_bits_uint8.o: u/t_bits_uint8.cpp
	@echo "        Compiling u/t_bits_uint8.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint8.o -c u/t_bits_uint8.cpp

u/t_bits_uint8.s: u/t_bits_uint8.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_uint8.s -S u/t_bits_uint8.cpp

u/t_bits_value.lo: u/t_bits_value.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_value.lo -c u/t_bits_value.cpp

u/t_bits_value.o: u/t_bits_value.cpp
	@echo "        Compiling u/t_bits_value.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_value.o -c u/t_bits_value.cpp

u/t_bits_value.s: u/t_bits_value.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_bits_value.s -S u/t_bits_value.cpp

u/t_charset_base64.lo: u/t_charset_base64.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_base64.lo -c u/t_charset_base64.cpp

u/t_charset_base64.o: u/t_charset_base64.cpp
	@echo "        Compiling u/t_charset_base64.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_base64.o -c u/t_charset_base64.cpp

u/t_charset_base64.s: u/t_charset_base64.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_base64.s -S u/t_charset_base64.cpp

u/t_charset_charset.lo: u/t_charset_charset.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charset.lo -c u/t_charset_charset.cpp

u/t_charset_charset.o: u/t_charset_charset.cpp
	@echo "        Compiling u/t_charset_charset.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charset.o -c u/t_charset_charset.cpp

u/t_charset_charset.s: u/t_charset_charset.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charset.s -S u/t_charset_charset.cpp

u/t_charset_charsetfactory.lo: u/t_charset_charsetfactory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charsetfactory.lo -c u/t_charset_charsetfactory.cpp

u/t_charset_charsetfactory.o: u/t_charset_charsetfactory.cpp
	@echo "        Compiling u/t_charset_charsetfactory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charsetfactory.o -c u/t_charset_charsetfactory.cpp

u/t_charset_charsetfactory.s: u/t_charset_charsetfactory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_charsetfactory.s -S u/t_charset_charsetfactory.cpp

u/t_charset_codepage.lo: u/t_charset_codepage.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepage.lo -c u/t_charset_codepage.cpp

u/t_charset_codepage.o: u/t_charset_codepage.cpp
	@echo "        Compiling u/t_charset_codepage.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepage.o -c u/t_charset_codepage.cpp

u/t_charset_codepage.s: u/t_charset_codepage.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepage.s -S u/t_charset_codepage.cpp

u/t_charset_codepagecharset.lo: u/t_charset_codepagecharset.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepagecharset.lo -c u/t_charset_codepagecharset.cpp

u/t_charset_codepagecharset.o: u/t_charset_codepagecharset.cpp
	@echo "        Compiling u/t_charset_codepagecharset.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepagecharset.o -c u/t_charset_codepagecharset.cpp

u/t_charset_codepagecharset.s: u/t_charset_codepagecharset.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_codepagecharset.s -S u/t_charset_codepagecharset.cpp

u/t_charset_defaultcharsetfactory.lo: u/t_charset_defaultcharsetfactory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_defaultcharsetfactory.lo -c u/t_charset_defaultcharsetfactory.cpp

u/t_charset_defaultcharsetfactory.o: u/t_charset_defaultcharsetfactory.cpp
	@echo "        Compiling u/t_charset_defaultcharsetfactory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_defaultcharsetfactory.o -c u/t_charset_defaultcharsetfactory.cpp

u/t_charset_defaultcharsetfactory.s: u/t_charset_defaultcharsetfactory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_defaultcharsetfactory.s -S u/t_charset_defaultcharsetfactory.cpp

u/t_charset_quotedprintable.lo: u/t_charset_quotedprintable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_quotedprintable.lo -c u/t_charset_quotedprintable.cpp

u/t_charset_quotedprintable.o: u/t_charset_quotedprintable.cpp
	@echo "        Compiling u/t_charset_quotedprintable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_quotedprintable.o -c u/t_charset_quotedprintable.cpp

u/t_charset_quotedprintable.s: u/t_charset_quotedprintable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_quotedprintable.s -S u/t_charset_quotedprintable.cpp

u/t_charset_unicode.lo: u/t_charset_unicode.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_unicode.lo -c u/t_charset_unicode.cpp

u/t_charset_unicode.o: u/t_charset_unicode.cpp
	@echo "        Compiling u/t_charset_unicode.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_unicode.o -c u/t_charset_unicode.cpp

u/t_charset_unicode.s: u/t_charset_unicode.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_unicode.s -S u/t_charset_unicode.cpp

u/t_charset_urlencoding.lo: u/t_charset_urlencoding.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_urlencoding.lo -c u/t_charset_urlencoding.cpp

u/t_charset_urlencoding.o: u/t_charset_urlencoding.cpp
	@echo "        Compiling u/t_charset_urlencoding.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_urlencoding.o -c u/t_charset_urlencoding.cpp

u/t_charset_urlencoding.s: u/t_charset_urlencoding.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_urlencoding.s -S u/t_charset_urlencoding.cpp

u/t_charset_utf8.lo: u/t_charset_utf8.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8.lo -c u/t_charset_utf8.cpp

u/t_charset_utf8.o: u/t_charset_utf8.cpp
	@echo "        Compiling u/t_charset_utf8.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8.o -c u/t_charset_utf8.cpp

u/t_charset_utf8.s: u/t_charset_utf8.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8.s -S u/t_charset_utf8.cpp

u/t_charset_utf8charset.lo: u/t_charset_utf8charset.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8charset.lo -c u/t_charset_utf8charset.cpp

u/t_charset_utf8charset.o: u/t_charset_utf8charset.cpp
	@echo "        Compiling u/t_charset_utf8charset.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8charset.o -c u/t_charset_utf8charset.cpp

u/t_charset_utf8charset.s: u/t_charset_utf8charset.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8charset.s -S u/t_charset_utf8charset.cpp

u/t_charset_utf8reader.lo: u/t_charset_utf8reader.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8reader.lo -c u/t_charset_utf8reader.cpp

u/t_charset_utf8reader.o: u/t_charset_utf8reader.cpp
	@echo "        Compiling u/t_charset_utf8reader.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8reader.o -c u/t_charset_utf8reader.cpp

u/t_charset_utf8reader.s: u/t_charset_utf8reader.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_charset_utf8reader.s -S u/t_charset_utf8reader.cpp

u/t_checksums_adler32.lo: u/t_checksums_adler32.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_adler32.lo -c u/t_checksums_adler32.cpp

u/t_checksums_adler32.o: u/t_checksums_adler32.cpp
	@echo "        Compiling u/t_checksums_adler32.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_adler32.o -c u/t_checksums_adler32.cpp

u/t_checksums_adler32.s: u/t_checksums_adler32.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_adler32.s -S u/t_checksums_adler32.cpp

u/t_checksums_bytesum.lo: u/t_checksums_bytesum.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_bytesum.lo -c u/t_checksums_bytesum.cpp

u/t_checksums_bytesum.o: u/t_checksums_bytesum.cpp
	@echo "        Compiling u/t_checksums_bytesum.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_bytesum.o -c u/t_checksums_bytesum.cpp

u/t_checksums_bytesum.s: u/t_checksums_bytesum.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_bytesum.s -S u/t_checksums_bytesum.cpp

u/t_checksums_checksum.lo: u/t_checksums_checksum.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_checksum.lo -c u/t_checksums_checksum.cpp

u/t_checksums_checksum.o: u/t_checksums_checksum.cpp
	@echo "        Compiling u/t_checksums_checksum.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_checksum.o -c u/t_checksums_checksum.cpp

u/t_checksums_checksum.s: u/t_checksums_checksum.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_checksum.s -S u/t_checksums_checksum.cpp

u/t_checksums_crc16.lo: u/t_checksums_crc16.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc16.lo -c u/t_checksums_crc16.cpp

u/t_checksums_crc16.o: u/t_checksums_crc16.cpp
	@echo "        Compiling u/t_checksums_crc16.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc16.o -c u/t_checksums_crc16.cpp

u/t_checksums_crc16.s: u/t_checksums_crc16.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc16.s -S u/t_checksums_crc16.cpp

u/t_checksums_crc32.lo: u/t_checksums_crc32.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc32.lo -c u/t_checksums_crc32.cpp

u/t_checksums_crc32.o: u/t_checksums_crc32.cpp
	@echo "        Compiling u/t_checksums_crc32.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc32.o -c u/t_checksums_crc32.cpp

u/t_checksums_crc32.s: u/t_checksums_crc32.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_crc32.s -S u/t_checksums_crc32.cpp

u/t_checksums_hash.lo: u/t_checksums_hash.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_hash.lo -c u/t_checksums_hash.cpp

u/t_checksums_hash.o: u/t_checksums_hash.cpp
	@echo "        Compiling u/t_checksums_hash.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_hash.o -c u/t_checksums_hash.cpp

u/t_checksums_hash.s: u/t_checksums_hash.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_hash.s -S u/t_checksums_hash.cpp

u/t_checksums_md5.lo: u/t_checksums_md5.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_md5.lo -c u/t_checksums_md5.cpp

u/t_checksums_md5.o: u/t_checksums_md5.cpp
	@echo "        Compiling u/t_checksums_md5.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_md5.o -c u/t_checksums_md5.cpp

u/t_checksums_md5.s: u/t_checksums_md5.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_md5.s -S u/t_checksums_md5.cpp

u/t_checksums_sha1.lo: u/t_checksums_sha1.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_sha1.lo -c u/t_checksums_sha1.cpp

u/t_checksums_sha1.o: u/t_checksums_sha1.cpp
	@echo "        Compiling u/t_checksums_sha1.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_sha1.o -c u/t_checksums_sha1.cpp

u/t_checksums_sha1.s: u/t_checksums_sha1.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_checksums_sha1.s -S u/t_checksums_sha1.cpp

u/t_container_dereferencingiterator.lo: \
    u/t_container_dereferencingiterator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_dereferencingiterator.lo -c u/t_container_dereferencingiterator.cpp

u/t_container_dereferencingiterator.o: \
    u/t_container_dereferencingiterator.cpp
	@echo "        Compiling u/t_container_dereferencingiterator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_dereferencingiterator.o -c u/t_container_dereferencingiterator.cpp

u/t_container_dereferencingiterator.s: \
    u/t_container_dereferencingiterator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_dereferencingiterator.s -S u/t_container_dereferencingiterator.cpp

u/t_container_memberiterator.lo: u/t_container_memberiterator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_memberiterator.lo -c u/t_container_memberiterator.cpp

u/t_container_memberiterator.o: u/t_container_memberiterator.cpp
	@echo "        Compiling u/t_container_memberiterator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_memberiterator.o -c u/t_container_memberiterator.cpp

u/t_container_memberiterator.s: u/t_container_memberiterator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_memberiterator.s -S u/t_container_memberiterator.cpp

u/t_container_ptrmap.lo: u/t_container_ptrmap.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmap.lo -c u/t_container_ptrmap.cpp

u/t_container_ptrmap.o: u/t_container_ptrmap.cpp
	@echo "        Compiling u/t_container_ptrmap.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmap.o -c u/t_container_ptrmap.cpp

u/t_container_ptrmap.s: u/t_container_ptrmap.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmap.s -S u/t_container_ptrmap.cpp

u/t_container_ptrmultilist.lo: u/t_container_ptrmultilist.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilist.lo -c u/t_container_ptrmultilist.cpp

u/t_container_ptrmultilist.o: u/t_container_ptrmultilist.cpp
	@echo "        Compiling u/t_container_ptrmultilist.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilist.o -c u/t_container_ptrmultilist.cpp

u/t_container_ptrmultilist.s: u/t_container_ptrmultilist.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilist.s -S u/t_container_ptrmultilist.cpp

u/t_container_ptrmultilistbase.lo: u/t_container_ptrmultilistbase.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilistbase.lo -c u/t_container_ptrmultilistbase.cpp

u/t_container_ptrmultilistbase.o: u/t_container_ptrmultilistbase.cpp
	@echo "        Compiling u/t_container_ptrmultilistbase.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilistbase.o -c u/t_container_ptrmultilistbase.cpp

u/t_container_ptrmultilistbase.s: u/t_container_ptrmultilistbase.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrmultilistbase.s -S u/t_container_ptrmultilistbase.cpp

u/t_container_ptrqueue.lo: u/t_container_ptrqueue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrqueue.lo -c u/t_container_ptrqueue.cpp

u/t_container_ptrqueue.o: u/t_container_ptrqueue.cpp
	@echo "        Compiling u/t_container_ptrqueue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrqueue.o -c u/t_container_ptrqueue.cpp

u/t_container_ptrqueue.s: u/t_container_ptrqueue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrqueue.s -S u/t_container_ptrqueue.cpp

u/t_container_ptrvector.lo: u/t_container_ptrvector.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrvector.lo -c u/t_container_ptrvector.cpp

u/t_container_ptrvector.o: u/t_container_ptrvector.cpp
	@echo "        Compiling u/t_container_ptrvector.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrvector.o -c u/t_container_ptrvector.cpp

u/t_container_ptrvector.s: u/t_container_ptrvector.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_container_ptrvector.s -S u/t_container_ptrvector.cpp

u/t_data_access.lo: u/t_data_access.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_access.lo -c u/t_data_access.cpp

u/t_data_access.o: u/t_data_access.cpp
	@echo "        Compiling u/t_data_access.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_access.o -c u/t_data_access.cpp

u/t_data_access.s: u/t_data_access.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_access.s -S u/t_data_access.cpp

u/t_data_booleanvalue.lo: u/t_data_booleanvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_booleanvalue.lo -c u/t_data_booleanvalue.cpp

u/t_data_booleanvalue.o: u/t_data_booleanvalue.cpp
	@echo "        Compiling u/t_data_booleanvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_booleanvalue.o -c u/t_data_booleanvalue.cpp

u/t_data_booleanvalue.s: u/t_data_booleanvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_booleanvalue.s -S u/t_data_booleanvalue.cpp

u/t_data_defaultvaluefactory.lo: u/t_data_defaultvaluefactory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_defaultvaluefactory.lo -c u/t_data_defaultvaluefactory.cpp

u/t_data_defaultvaluefactory.o: u/t_data_defaultvaluefactory.cpp
	@echo "        Compiling u/t_data_defaultvaluefactory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_defaultvaluefactory.o -c u/t_data_defaultvaluefactory.cpp

u/t_data_defaultvaluefactory.s: u/t_data_defaultvaluefactory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_defaultvaluefactory.s -S u/t_data_defaultvaluefactory.cpp

u/t_data_errorvalue.lo: u/t_data_errorvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_errorvalue.lo -c u/t_data_errorvalue.cpp

u/t_data_errorvalue.o: u/t_data_errorvalue.cpp
	@echo "        Compiling u/t_data_errorvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_errorvalue.o -c u/t_data_errorvalue.cpp

u/t_data_errorvalue.s: u/t_data_errorvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_errorvalue.s -S u/t_data_errorvalue.cpp

u/t_data_floatvalue.lo: u/t_data_floatvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_floatvalue.lo -c u/t_data_floatvalue.cpp

u/t_data_floatvalue.o: u/t_data_floatvalue.cpp
	@echo "        Compiling u/t_data_floatvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_floatvalue.o -c u/t_data_floatvalue.cpp

u/t_data_floatvalue.s: u/t_data_floatvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_floatvalue.s -S u/t_data_floatvalue.cpp

u/t_data_hash.lo: u/t_data_hash.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hash.lo -c u/t_data_hash.cpp

u/t_data_hash.o: u/t_data_hash.cpp
	@echo "        Compiling u/t_data_hash.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hash.o -c u/t_data_hash.cpp

u/t_data_hash.s: u/t_data_hash.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hash.s -S u/t_data_hash.cpp

u/t_data_hashvalue.lo: u/t_data_hashvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hashvalue.lo -c u/t_data_hashvalue.cpp

u/t_data_hashvalue.o: u/t_data_hashvalue.cpp
	@echo "        Compiling u/t_data_hashvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hashvalue.o -c u/t_data_hashvalue.cpp

u/t_data_hashvalue.s: u/t_data_hashvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_hashvalue.s -S u/t_data_hashvalue.cpp

u/t_data_integerlist.lo: u/t_data_integerlist.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integerlist.lo -c u/t_data_integerlist.cpp

u/t_data_integerlist.o: u/t_data_integerlist.cpp
	@echo "        Compiling u/t_data_integerlist.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integerlist.o -c u/t_data_integerlist.cpp

u/t_data_integerlist.s: u/t_data_integerlist.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integerlist.s -S u/t_data_integerlist.cpp

u/t_data_integervalue.lo: u/t_data_integervalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integervalue.lo -c u/t_data_integervalue.cpp

u/t_data_integervalue.o: u/t_data_integervalue.cpp
	@echo "        Compiling u/t_data_integervalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integervalue.o -c u/t_data_integervalue.cpp

u/t_data_integervalue.s: u/t_data_integervalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_integervalue.s -S u/t_data_integervalue.cpp

u/t_data_namemap.lo: u/t_data_namemap.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namemap.lo -c u/t_data_namemap.cpp

u/t_data_namemap.o: u/t_data_namemap.cpp
	@echo "        Compiling u/t_data_namemap.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namemap.o -c u/t_data_namemap.cpp

u/t_data_namemap.s: u/t_data_namemap.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namemap.s -S u/t_data_namemap.cpp

u/t_data_namequery.lo: u/t_data_namequery.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namequery.lo -c u/t_data_namequery.cpp

u/t_data_namequery.o: u/t_data_namequery.cpp
	@echo "        Compiling u/t_data_namequery.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namequery.o -c u/t_data_namequery.cpp

u/t_data_namequery.s: u/t_data_namequery.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_namequery.s -S u/t_data_namequery.cpp

u/t_data_scalarvalue.lo: u/t_data_scalarvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_scalarvalue.lo -c u/t_data_scalarvalue.cpp

u/t_data_scalarvalue.o: u/t_data_scalarvalue.cpp
	@echo "        Compiling u/t_data_scalarvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_scalarvalue.o -c u/t_data_scalarvalue.cpp

u/t_data_scalarvalue.s: u/t_data_scalarvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_scalarvalue.s -S u/t_data_scalarvalue.cpp

u/t_data_segment.lo: u/t_data_segment.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segment.lo -c u/t_data_segment.cpp

u/t_data_segment.o: u/t_data_segment.cpp
	@echo "        Compiling u/t_data_segment.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segment.o -c u/t_data_segment.cpp

u/t_data_segment.s: u/t_data_segment.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segment.s -S u/t_data_segment.cpp

u/t_data_segmentview.lo: u/t_data_segmentview.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segmentview.lo -c u/t_data_segmentview.cpp

u/t_data_segmentview.o: u/t_data_segmentview.cpp
	@echo "        Compiling u/t_data_segmentview.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segmentview.o -c u/t_data_segmentview.cpp

u/t_data_segmentview.s: u/t_data_segmentview.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_segmentview.s -S u/t_data_segmentview.cpp

u/t_data_stringlist.lo: u/t_data_stringlist.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringlist.lo -c u/t_data_stringlist.cpp

u/t_data_stringlist.o: u/t_data_stringlist.cpp
	@echo "        Compiling u/t_data_stringlist.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringlist.o -c u/t_data_stringlist.cpp

u/t_data_stringlist.s: u/t_data_stringlist.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringlist.s -S u/t_data_stringlist.cpp

u/t_data_stringvalue.lo: u/t_data_stringvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringvalue.lo -c u/t_data_stringvalue.cpp

u/t_data_stringvalue.o: u/t_data_stringvalue.cpp
	@echo "        Compiling u/t_data_stringvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringvalue.o -c u/t_data_stringvalue.cpp

u/t_data_stringvalue.s: u/t_data_stringvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_stringvalue.s -S u/t_data_stringvalue.cpp

u/t_data_value.lo: u/t_data_value.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_value.lo -c u/t_data_value.cpp

u/t_data_value.o: u/t_data_value.cpp
	@echo "        Compiling u/t_data_value.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_value.o -c u/t_data_value.cpp

u/t_data_value.s: u/t_data_value.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_value.s -S u/t_data_value.cpp

u/t_data_valuefactory.lo: u/t_data_valuefactory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_valuefactory.lo -c u/t_data_valuefactory.cpp

u/t_data_valuefactory.o: u/t_data_valuefactory.cpp
	@echo "        Compiling u/t_data_valuefactory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_valuefactory.o -c u/t_data_valuefactory.cpp

u/t_data_valuefactory.s: u/t_data_valuefactory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_valuefactory.s -S u/t_data_valuefactory.cpp

u/t_data_vector.lo: u/t_data_vector.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vector.lo -c u/t_data_vector.cpp

u/t_data_vector.o: u/t_data_vector.cpp
	@echo "        Compiling u/t_data_vector.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vector.o -c u/t_data_vector.cpp

u/t_data_vector.s: u/t_data_vector.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vector.s -S u/t_data_vector.cpp

u/t_data_vectorvalue.lo: u/t_data_vectorvalue.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vectorvalue.lo -c u/t_data_vectorvalue.cpp

u/t_data_vectorvalue.o: u/t_data_vectorvalue.cpp
	@echo "        Compiling u/t_data_vectorvalue.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vectorvalue.o -c u/t_data_vectorvalue.cpp

u/t_data_vectorvalue.s: u/t_data_vectorvalue.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_vectorvalue.s -S u/t_data_vectorvalue.cpp

u/t_data_visitor.lo: u/t_data_visitor.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_visitor.lo -c u/t_data_visitor.cpp

u/t_data_visitor.o: u/t_data_visitor.cpp
	@echo "        Compiling u/t_data_visitor.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_visitor.o -c u/t_data_visitor.cpp

u/t_data_visitor.s: u/t_data_visitor.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_data_visitor.s -S u/t_data_visitor.cpp

u/t_except.lo: u/t_except.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except.lo -c u/t_except.cpp

u/t_except.o: u/t_except.cpp
	@echo "        Compiling u/t_except.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except.o -c u/t_except.cpp

u/t_except.s: u/t_except.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except.s -S u/t_except.cpp

u/t_except_assertionfailedexception.lo: \
    u/t_except_assertionfailedexception.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except_assertionfailedexception.lo -c u/t_except_assertionfailedexception.cpp

u/t_except_assertionfailedexception.o: \
    u/t_except_assertionfailedexception.cpp
	@echo "        Compiling u/t_except_assertionfailedexception.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except_assertionfailedexception.o -c u/t_except_assertionfailedexception.cpp

u/t_except_assertionfailedexception.s: \
    u/t_except_assertionfailedexception.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_except_assertionfailedexception.s -S u/t_except_assertionfailedexception.cpp

u/t_functional_binaryfunction.lo: u/t_functional_binaryfunction.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_binaryfunction.lo -c u/t_functional_binaryfunction.cpp

u/t_functional_binaryfunction.o: u/t_functional_binaryfunction.cpp
	@echo "        Compiling u/t_functional_binaryfunction.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_binaryfunction.o -c u/t_functional_binaryfunction.cpp

u/t_functional_binaryfunction.s: u/t_functional_binaryfunction.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_binaryfunction.s -S u/t_functional_binaryfunction.cpp

u/t_functional_mapping.lo: u/t_functional_mapping.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_mapping.lo -c u/t_functional_mapping.cpp

u/t_functional_mapping.o: u/t_functional_mapping.cpp
	@echo "        Compiling u/t_functional_mapping.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_mapping.o -c u/t_functional_mapping.cpp

u/t_functional_mapping.s: u/t_functional_mapping.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_mapping.s -S u/t_functional_mapping.cpp

u/t_functional_stringtable.lo: u/t_functional_stringtable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_stringtable.lo -c u/t_functional_stringtable.cpp

u/t_functional_stringtable.o: u/t_functional_stringtable.cpp
	@echo "        Compiling u/t_functional_stringtable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_stringtable.o -c u/t_functional_stringtable.cpp

u/t_functional_stringtable.s: u/t_functional_stringtable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_stringtable.s -S u/t_functional_stringtable.cpp

u/t_functional_unaryfunction.lo: u/t_functional_unaryfunction.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_unaryfunction.lo -c u/t_functional_unaryfunction.cpp

u/t_functional_unaryfunction.o: u/t_functional_unaryfunction.cpp
	@echo "        Compiling u/t_functional_unaryfunction.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_unaryfunction.o -c u/t_functional_unaryfunction.cpp

u/t_functional_unaryfunction.s: u/t_functional_unaryfunction.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_functional_unaryfunction.s -S u/t_functional_unaryfunction.cpp

u/t_io_archive_zipreader.lo: u/t_io_archive_zipreader.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_archive_zipreader.lo -c u/t_io_archive_zipreader.cpp

u/t_io_archive_zipreader.o: u/t_io_archive_zipreader.cpp
	@echo "        Compiling u/t_io_archive_zipreader.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_archive_zipreader.o -c u/t_io_archive_zipreader.cpp

u/t_io_archive_zipreader.s: u/t_io_archive_zipreader.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_archive_zipreader.s -S u/t_io_archive_zipreader.cpp

u/t_io_bufferedsink.lo: u/t_io_bufferedsink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedsink.lo -c u/t_io_bufferedsink.cpp

u/t_io_bufferedsink.o: u/t_io_bufferedsink.cpp
	@echo "        Compiling u/t_io_bufferedsink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedsink.o -c u/t_io_bufferedsink.cpp

u/t_io_bufferedsink.s: u/t_io_bufferedsink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedsink.s -S u/t_io_bufferedsink.cpp

u/t_io_bufferedstream.lo: u/t_io_bufferedstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedstream.lo -c u/t_io_bufferedstream.cpp

u/t_io_bufferedstream.o: u/t_io_bufferedstream.cpp
	@echo "        Compiling u/t_io_bufferedstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedstream.o -c u/t_io_bufferedstream.cpp

u/t_io_bufferedstream.s: u/t_io_bufferedstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_bufferedstream.s -S u/t_io_bufferedstream.cpp

u/t_io_constmemorystream.lo: u/t_io_constmemorystream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_constmemorystream.lo -c u/t_io_constmemorystream.cpp

u/t_io_constmemorystream.o: u/t_io_constmemorystream.cpp
	@echo "        Compiling u/t_io_constmemorystream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_constmemorystream.o -c u/t_io_constmemorystream.cpp

u/t_io_constmemorystream.s: u/t_io_constmemorystream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_constmemorystream.s -S u/t_io_constmemorystream.cpp

u/t_io_datasink.lo: u/t_io_datasink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_datasink.lo -c u/t_io_datasink.cpp

u/t_io_datasink.o: u/t_io_datasink.cpp
	@echo "        Compiling u/t_io_datasink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_datasink.o -c u/t_io_datasink.cpp

u/t_io_datasink.s: u/t_io_datasink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_datasink.s -S u/t_io_datasink.cpp

u/t_io_deflatetransform.lo: u/t_io_deflatetransform.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_deflatetransform.lo -c u/t_io_deflatetransform.cpp

u/t_io_deflatetransform.o: u/t_io_deflatetransform.cpp
	@echo "        Compiling u/t_io_deflatetransform.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_deflatetransform.o -c u/t_io_deflatetransform.cpp

u/t_io_deflatetransform.s: u/t_io_deflatetransform.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_deflatetransform.s -S u/t_io_deflatetransform.cpp

u/t_io_directory.lo: u/t_io_directory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directory.lo -c u/t_io_directory.cpp

u/t_io_directory.o: u/t_io_directory.cpp
	@echo "        Compiling u/t_io_directory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directory.o -c u/t_io_directory.cpp

u/t_io_directory.s: u/t_io_directory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directory.s -S u/t_io_directory.cpp

u/t_io_directoryentry.lo: u/t_io_directoryentry.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directoryentry.lo -c u/t_io_directoryentry.cpp

u/t_io_directoryentry.o: u/t_io_directoryentry.cpp
	@echo "        Compiling u/t_io_directoryentry.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directoryentry.o -c u/t_io_directoryentry.cpp

u/t_io_directoryentry.s: u/t_io_directoryentry.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_directoryentry.s -S u/t_io_directoryentry.cpp

u/t_io_filemapping.lo: u/t_io_filemapping.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filemapping.lo -c u/t_io_filemapping.cpp

u/t_io_filemapping.o: u/t_io_filemapping.cpp
	@echo "        Compiling u/t_io_filemapping.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filemapping.o -c u/t_io_filemapping.cpp

u/t_io_filemapping.s: u/t_io_filemapping.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filemapping.s -S u/t_io_filemapping.cpp

u/t_io_filesystem.lo: u/t_io_filesystem.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filesystem.lo -c u/t_io_filesystem.cpp

u/t_io_filesystem.o: u/t_io_filesystem.cpp
	@echo "        Compiling u/t_io_filesystem.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filesystem.o -c u/t_io_filesystem.cpp

u/t_io_filesystem.s: u/t_io_filesystem.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_filesystem.s -S u/t_io_filesystem.cpp

u/t_io_inflatedatasink.lo: u/t_io_inflatedatasink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatedatasink.lo -c u/t_io_inflatedatasink.cpp

u/t_io_inflatedatasink.o: u/t_io_inflatedatasink.cpp
	@echo "        Compiling u/t_io_inflatedatasink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatedatasink.o -c u/t_io_inflatedatasink.cpp

u/t_io_inflatedatasink.s: u/t_io_inflatedatasink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatedatasink.s -S u/t_io_inflatedatasink.cpp

u/t_io_inflatetransform.lo: u/t_io_inflatetransform.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatetransform.lo -c u/t_io_inflatetransform.cpp

u/t_io_inflatetransform.o: u/t_io_inflatetransform.cpp
	@echo "        Compiling u/t_io_inflatetransform.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatetransform.o -c u/t_io_inflatetransform.cpp

u/t_io_inflatetransform.s: u/t_io_inflatetransform.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_inflatetransform.s -S u/t_io_inflatetransform.cpp

u/t_io_internaldirectory.lo: u/t_io_internaldirectory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internaldirectory.lo -c u/t_io_internaldirectory.cpp

u/t_io_internaldirectory.o: u/t_io_internaldirectory.cpp
	@echo "        Compiling u/t_io_internaldirectory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internaldirectory.o -c u/t_io_internaldirectory.cpp

u/t_io_internaldirectory.s: u/t_io_internaldirectory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internaldirectory.s -S u/t_io_internaldirectory.cpp

u/t_io_internalfilemapping.lo: u/t_io_internalfilemapping.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalfilemapping.lo -c u/t_io_internalfilemapping.cpp

u/t_io_internalfilemapping.o: u/t_io_internalfilemapping.cpp
	@echo "        Compiling u/t_io_internalfilemapping.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalfilemapping.o -c u/t_io_internalfilemapping.cpp

u/t_io_internalfilemapping.s: u/t_io_internalfilemapping.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalfilemapping.s -S u/t_io_internalfilemapping.cpp

u/t_io_internalsink.lo: u/t_io_internalsink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalsink.lo -c u/t_io_internalsink.cpp

u/t_io_internalsink.o: u/t_io_internalsink.cpp
	@echo "        Compiling u/t_io_internalsink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalsink.o -c u/t_io_internalsink.cpp

u/t_io_internalsink.s: u/t_io_internalsink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalsink.s -S u/t_io_internalsink.cpp

u/t_io_internalstream.lo: u/t_io_internalstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalstream.lo -c u/t_io_internalstream.cpp

u/t_io_internalstream.o: u/t_io_internalstream.cpp
	@echo "        Compiling u/t_io_internalstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalstream.o -c u/t_io_internalstream.cpp

u/t_io_internalstream.s: u/t_io_internalstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_internalstream.s -S u/t_io_internalstream.cpp

u/t_io_json_parser.lo: u/t_io_json_parser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser.lo -c u/t_io_json_parser.cpp

u/t_io_json_parser.o: u/t_io_json_parser.cpp
	@echo "        Compiling u/t_io_json_parser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser.o -c u/t_io_json_parser.cpp

u/t_io_json_parser.s: u/t_io_json_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser.s -S u/t_io_json_parser.cpp

u/t_io_json_parser_testsuite.lo: u/t_io_json_parser_testsuite.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser_testsuite.lo -c u/t_io_json_parser_testsuite.cpp

u/t_io_json_parser_testsuite.o: u/t_io_json_parser_testsuite.cpp
	@echo "        Compiling u/t_io_json_parser_testsuite.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser_testsuite.o -c u/t_io_json_parser_testsuite.cpp

u/t_io_json_parser_testsuite.s: u/t_io_json_parser_testsuite.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_parser_testsuite.s -S u/t_io_json_parser_testsuite.cpp

u/t_io_json_writer.lo: u/t_io_json_writer.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_writer.lo -c u/t_io_json_writer.cpp

u/t_io_json_writer.o: u/t_io_json_writer.cpp
	@echo "        Compiling u/t_io_json_writer.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_writer.o -c u/t_io_json_writer.cpp

u/t_io_json_writer.s: u/t_io_json_writer.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_json_writer.s -S u/t_io_json_writer.cpp

u/t_io_limiteddatasink.lo: u/t_io_limiteddatasink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limiteddatasink.lo -c u/t_io_limiteddatasink.cpp

u/t_io_limiteddatasink.o: u/t_io_limiteddatasink.cpp
	@echo "        Compiling u/t_io_limiteddatasink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limiteddatasink.o -c u/t_io_limiteddatasink.cpp

u/t_io_limiteddatasink.s: u/t_io_limiteddatasink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limiteddatasink.s -S u/t_io_limiteddatasink.cpp

u/t_io_limitedstream.lo: u/t_io_limitedstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limitedstream.lo -c u/t_io_limitedstream.cpp

u/t_io_limitedstream.o: u/t_io_limitedstream.cpp
	@echo "        Compiling u/t_io_limitedstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limitedstream.o -c u/t_io_limitedstream.cpp

u/t_io_limitedstream.s: u/t_io_limitedstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_limitedstream.s -S u/t_io_limitedstream.cpp

u/t_io_memorystream.lo: u/t_io_memorystream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_memorystream.lo -c u/t_io_memorystream.cpp

u/t_io_memorystream.o: u/t_io_memorystream.cpp
	@echo "        Compiling u/t_io_memorystream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_memorystream.o -c u/t_io_memorystream.cpp

u/t_io_memorystream.s: u/t_io_memorystream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_memorystream.s -S u/t_io_memorystream.cpp

u/t_io_msexpandtransform.lo: u/t_io_msexpandtransform.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_msexpandtransform.lo -c u/t_io_msexpandtransform.cpp

u/t_io_msexpandtransform.o: u/t_io_msexpandtransform.cpp
	@echo "        Compiling u/t_io_msexpandtransform.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_msexpandtransform.o -c u/t_io_msexpandtransform.cpp

u/t_io_msexpandtransform.s: u/t_io_msexpandtransform.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_msexpandtransform.s -S u/t_io_msexpandtransform.cpp

u/t_io_multidirectory.lo: u/t_io_multidirectory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multidirectory.lo -c u/t_io_multidirectory.cpp

u/t_io_multidirectory.o: u/t_io_multidirectory.cpp
	@echo "        Compiling u/t_io_multidirectory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multidirectory.o -c u/t_io_multidirectory.cpp

u/t_io_multidirectory.s: u/t_io_multidirectory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multidirectory.s -S u/t_io_multidirectory.cpp

u/t_io_multiplexablestream.lo: u/t_io_multiplexablestream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multiplexablestream.lo -c u/t_io_multiplexablestream.cpp

u/t_io_multiplexablestream.o: u/t_io_multiplexablestream.cpp
	@echo "        Compiling u/t_io_multiplexablestream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multiplexablestream.o -c u/t_io_multiplexablestream.cpp

u/t_io_multiplexablestream.s: u/t_io_multiplexablestream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_multiplexablestream.s -S u/t_io_multiplexablestream.cpp

u/t_io_nullfilesystem.lo: u/t_io_nullfilesystem.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullfilesystem.lo -c u/t_io_nullfilesystem.cpp

u/t_io_nullfilesystem.o: u/t_io_nullfilesystem.cpp
	@echo "        Compiling u/t_io_nullfilesystem.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullfilesystem.o -c u/t_io_nullfilesystem.cpp

u/t_io_nullfilesystem.s: u/t_io_nullfilesystem.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullfilesystem.s -S u/t_io_nullfilesystem.cpp

u/t_io_nullstream.lo: u/t_io_nullstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullstream.lo -c u/t_io_nullstream.cpp

u/t_io_nullstream.o: u/t_io_nullstream.cpp
	@echo "        Compiling u/t_io_nullstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullstream.o -c u/t_io_nullstream.cpp

u/t_io_nullstream.s: u/t_io_nullstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nullstream.s -S u/t_io_nullstream.cpp

u/t_io_nulltextwriter.lo: u/t_io_nulltextwriter.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nulltextwriter.lo -c u/t_io_nulltextwriter.cpp

u/t_io_nulltextwriter.o: u/t_io_nulltextwriter.cpp
	@echo "        Compiling u/t_io_nulltextwriter.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nulltextwriter.o -c u/t_io_nulltextwriter.cpp

u/t_io_nulltextwriter.s: u/t_io_nulltextwriter.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_nulltextwriter.s -S u/t_io_nulltextwriter.cpp

u/t_io_resp_parser.lo: u/t_io_resp_parser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_parser.lo -c u/t_io_resp_parser.cpp

u/t_io_resp_parser.o: u/t_io_resp_parser.cpp
	@echo "        Compiling u/t_io_resp_parser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_parser.o -c u/t_io_resp_parser.cpp

u/t_io_resp_parser.s: u/t_io_resp_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_parser.s -S u/t_io_resp_parser.cpp

u/t_io_resp_writer.lo: u/t_io_resp_writer.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_writer.lo -c u/t_io_resp_writer.cpp

u/t_io_resp_writer.o: u/t_io_resp_writer.cpp
	@echo "        Compiling u/t_io_resp_writer.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_writer.o -c u/t_io_resp_writer.cpp

u/t_io_resp_writer.s: u/t_io_resp_writer.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_resp_writer.s -S u/t_io_resp_writer.cpp

u/t_io_stream.lo: u/t_io_stream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_stream.lo -c u/t_io_stream.cpp

u/t_io_stream.o: u/t_io_stream.cpp
	@echo "        Compiling u/t_io_stream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_stream.o -c u/t_io_stream.cpp

u/t_io_stream.s: u/t_io_stream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_stream.s -S u/t_io_stream.cpp

u/t_io_textfile.lo: u/t_io_textfile.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textfile.lo -c u/t_io_textfile.cpp

u/t_io_textfile.o: u/t_io_textfile.cpp
	@echo "        Compiling u/t_io_textfile.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textfile.o -c u/t_io_textfile.cpp

u/t_io_textfile.s: u/t_io_textfile.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textfile.s -S u/t_io_textfile.cpp

u/t_io_textreader.lo: u/t_io_textreader.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textreader.lo -c u/t_io_textreader.cpp

u/t_io_textreader.o: u/t_io_textreader.cpp
	@echo "        Compiling u/t_io_textreader.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textreader.o -c u/t_io_textreader.cpp

u/t_io_textreader.s: u/t_io_textreader.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textreader.s -S u/t_io_textreader.cpp

u/t_io_textsink.lo: u/t_io_textsink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textsink.lo -c u/t_io_textsink.cpp

u/t_io_textsink.o: u/t_io_textsink.cpp
	@echo "        Compiling u/t_io_textsink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textsink.o -c u/t_io_textsink.cpp

u/t_io_textsink.s: u/t_io_textsink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textsink.s -S u/t_io_textsink.cpp

u/t_io_textwriter.lo: u/t_io_textwriter.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textwriter.lo -c u/t_io_textwriter.cpp

u/t_io_textwriter.o: u/t_io_textwriter.cpp
	@echo "        Compiling u/t_io_textwriter.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textwriter.o -c u/t_io_textwriter.cpp

u/t_io_textwriter.s: u/t_io_textwriter.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_textwriter.s -S u/t_io_textwriter.cpp

u/t_io_transform.lo: u/t_io_transform.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transform.lo -c u/t_io_transform.cpp

u/t_io_transform.o: u/t_io_transform.cpp
	@echo "        Compiling u/t_io_transform.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transform.o -c u/t_io_transform.cpp

u/t_io_transform.s: u/t_io_transform.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transform.s -S u/t_io_transform.cpp

u/t_io_transformdatasink.lo: u/t_io_transformdatasink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformdatasink.lo -c u/t_io_transformdatasink.cpp

u/t_io_transformdatasink.o: u/t_io_transformdatasink.cpp
	@echo "        Compiling u/t_io_transformdatasink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformdatasink.o -c u/t_io_transformdatasink.cpp

u/t_io_transformdatasink.s: u/t_io_transformdatasink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformdatasink.s -S u/t_io_transformdatasink.cpp

u/t_io_transformreaderstream.lo: u/t_io_transformreaderstream.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformreaderstream.lo -c u/t_io_transformreaderstream.cpp

u/t_io_transformreaderstream.o: u/t_io_transformreaderstream.cpp
	@echo "        Compiling u/t_io_transformreaderstream.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformreaderstream.o -c u/t_io_transformreaderstream.cpp

u/t_io_transformreaderstream.s: u/t_io_transformreaderstream.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_transformreaderstream.s -S u/t_io_transformreaderstream.cpp

u/t_io_xml_basereader.lo: u/t_io_xml_basereader.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_basereader.lo -c u/t_io_xml_basereader.cpp

u/t_io_xml_basereader.o: u/t_io_xml_basereader.cpp
	@echo "        Compiling u/t_io_xml_basereader.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_basereader.o -c u/t_io_xml_basereader.cpp

u/t_io_xml_basereader.s: u/t_io_xml_basereader.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_basereader.s -S u/t_io_xml_basereader.cpp

u/t_io_xml_defaultentityhandler.lo: u/t_io_xml_defaultentityhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_defaultentityhandler.lo -c u/t_io_xml_defaultentityhandler.cpp

u/t_io_xml_defaultentityhandler.o: u/t_io_xml_defaultentityhandler.cpp
	@echo "        Compiling u/t_io_xml_defaultentityhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_defaultentityhandler.o -c u/t_io_xml_defaultentityhandler.cpp

u/t_io_xml_defaultentityhandler.s: u/t_io_xml_defaultentityhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_defaultentityhandler.s -S u/t_io_xml_defaultentityhandler.cpp

u/t_io_xml_entityhandler.lo: u/t_io_xml_entityhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_entityhandler.lo -c u/t_io_xml_entityhandler.cpp

u/t_io_xml_entityhandler.o: u/t_io_xml_entityhandler.cpp
	@echo "        Compiling u/t_io_xml_entityhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_entityhandler.o -c u/t_io_xml_entityhandler.cpp

u/t_io_xml_entityhandler.s: u/t_io_xml_entityhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_entityhandler.s -S u/t_io_xml_entityhandler.cpp

u/t_io_xml_reader.lo: u/t_io_xml_reader.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_reader.lo -c u/t_io_xml_reader.cpp

u/t_io_xml_reader.o: u/t_io_xml_reader.cpp
	@echo "        Compiling u/t_io_xml_reader.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_reader.o -c u/t_io_xml_reader.cpp

u/t_io_xml_reader.s: u/t_io_xml_reader.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_io_xml_reader.s -S u/t_io_xml_reader.cpp

u/t_net_acceptoperation.lo: u/t_net_acceptoperation.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_acceptoperation.lo -c u/t_net_acceptoperation.cpp

u/t_net_acceptoperation.o: u/t_net_acceptoperation.cpp
	@echo "        Compiling u/t_net_acceptoperation.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_acceptoperation.o -c u/t_net_acceptoperation.cpp

u/t_net_acceptoperation.s: u/t_net_acceptoperation.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_acceptoperation.s -S u/t_net_acceptoperation.cpp

u/t_net_commandhandler.lo: u/t_net_commandhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_commandhandler.lo -c u/t_net_commandhandler.cpp

u/t_net_commandhandler.o: u/t_net_commandhandler.cpp
	@echo "        Compiling u/t_net_commandhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_commandhandler.o -c u/t_net_commandhandler.cpp

u/t_net_commandhandler.s: u/t_net_commandhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_commandhandler.s -S u/t_net_commandhandler.cpp

u/t_net_headerconsumer.lo: u/t_net_headerconsumer.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerconsumer.lo -c u/t_net_headerconsumer.cpp

u/t_net_headerconsumer.o: u/t_net_headerconsumer.cpp
	@echo "        Compiling u/t_net_headerconsumer.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerconsumer.o -c u/t_net_headerconsumer.cpp

u/t_net_headerconsumer.s: u/t_net_headerconsumer.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerconsumer.s -S u/t_net_headerconsumer.cpp

u/t_net_headerfield.lo: u/t_net_headerfield.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerfield.lo -c u/t_net_headerfield.cpp

u/t_net_headerfield.o: u/t_net_headerfield.cpp
	@echo "        Compiling u/t_net_headerfield.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerfield.o -c u/t_net_headerfield.cpp

u/t_net_headerfield.s: u/t_net_headerfield.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerfield.s -S u/t_net_headerfield.cpp

u/t_net_headerparser.lo: u/t_net_headerparser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerparser.lo -c u/t_net_headerparser.cpp

u/t_net_headerparser.o: u/t_net_headerparser.cpp
	@echo "        Compiling u/t_net_headerparser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerparser.o -c u/t_net_headerparser.cpp

u/t_net_headerparser.s: u/t_net_headerparser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headerparser.s -S u/t_net_headerparser.cpp

u/t_net_headertable.lo: u/t_net_headertable.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headertable.lo -c u/t_net_headertable.cpp

u/t_net_headertable.o: u/t_net_headertable.cpp
	@echo "        Compiling u/t_net_headertable.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headertable.o -c u/t_net_headertable.cpp

u/t_net_headertable.s: u/t_net_headertable.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_headertable.s -S u/t_net_headertable.cpp

u/t_net_http_chunkedsink.lo: u/t_net_http_chunkedsink.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_chunkedsink.lo -c u/t_net_http_chunkedsink.cpp

u/t_net_http_chunkedsink.o: u/t_net_http_chunkedsink.cpp
	@echo "        Compiling u/t_net_http_chunkedsink.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_chunkedsink.o -c u/t_net_http_chunkedsink.cpp

u/t_net_http_chunkedsink.s: u/t_net_http_chunkedsink.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_chunkedsink.s -S u/t_net_http_chunkedsink.cpp

u/t_net_http_client.lo: u/t_net_http_client.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_client.lo -c u/t_net_http_client.cpp

u/t_net_http_client.o: u/t_net_http_client.cpp
	@echo "        Compiling u/t_net_http_client.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_client.o -c u/t_net_http_client.cpp

u/t_net_http_client.s: u/t_net_http_client.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_client.s -S u/t_net_http_client.cpp

u/t_net_http_clientconnection.lo: u/t_net_http_clientconnection.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnection.lo -c u/t_net_http_clientconnection.cpp

u/t_net_http_clientconnection.o: u/t_net_http_clientconnection.cpp
	@echo "        Compiling u/t_net_http_clientconnection.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnection.o -c u/t_net_http_clientconnection.cpp

u/t_net_http_clientconnection.s: u/t_net_http_clientconnection.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnection.s -S u/t_net_http_clientconnection.cpp

u/t_net_http_clientconnectionprovider.lo: \
    u/t_net_http_clientconnectionprovider.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnectionprovider.lo -c u/t_net_http_clientconnectionprovider.cpp

u/t_net_http_clientconnectionprovider.o: \
    u/t_net_http_clientconnectionprovider.cpp
	@echo "        Compiling u/t_net_http_clientconnectionprovider.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnectionprovider.o -c u/t_net_http_clientconnectionprovider.cpp

u/t_net_http_clientconnectionprovider.s: \
    u/t_net_http_clientconnectionprovider.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientconnectionprovider.s -S u/t_net_http_clientconnectionprovider.cpp

u/t_net_http_clientrequest.lo: u/t_net_http_clientrequest.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientrequest.lo -c u/t_net_http_clientrequest.cpp

u/t_net_http_clientrequest.o: u/t_net_http_clientrequest.cpp
	@echo "        Compiling u/t_net_http_clientrequest.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientrequest.o -c u/t_net_http_clientrequest.cpp

u/t_net_http_clientrequest.s: u/t_net_http_clientrequest.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientrequest.s -S u/t_net_http_clientrequest.cpp

u/t_net_http_clientresponse.lo: u/t_net_http_clientresponse.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientresponse.lo -c u/t_net_http_clientresponse.cpp

u/t_net_http_clientresponse.o: u/t_net_http_clientresponse.cpp
	@echo "        Compiling u/t_net_http_clientresponse.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientresponse.o -c u/t_net_http_clientresponse.cpp

u/t_net_http_clientresponse.s: u/t_net_http_clientresponse.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_clientresponse.s -S u/t_net_http_clientresponse.cpp

u/t_net_http_cookie.lo: u/t_net_http_cookie.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookie.lo -c u/t_net_http_cookie.cpp

u/t_net_http_cookie.o: u/t_net_http_cookie.cpp
	@echo "        Compiling u/t_net_http_cookie.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookie.o -c u/t_net_http_cookie.cpp

u/t_net_http_cookie.s: u/t_net_http_cookie.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookie.s -S u/t_net_http_cookie.cpp

u/t_net_http_cookiejar.lo: u/t_net_http_cookiejar.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookiejar.lo -c u/t_net_http_cookiejar.cpp

u/t_net_http_cookiejar.o: u/t_net_http_cookiejar.cpp
	@echo "        Compiling u/t_net_http_cookiejar.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookiejar.o -c u/t_net_http_cookiejar.cpp

u/t_net_http_cookiejar.s: u/t_net_http_cookiejar.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_cookiejar.s -S u/t_net_http_cookiejar.cpp

u/t_net_http_defaultconnectionprovider.lo: \
    u/t_net_http_defaultconnectionprovider.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_defaultconnectionprovider.lo -c u/t_net_http_defaultconnectionprovider.cpp

u/t_net_http_defaultconnectionprovider.o: \
    u/t_net_http_defaultconnectionprovider.cpp
	@echo "        Compiling u/t_net_http_defaultconnectionprovider.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_defaultconnectionprovider.o -c u/t_net_http_defaultconnectionprovider.cpp

u/t_net_http_defaultconnectionprovider.s: \
    u/t_net_http_defaultconnectionprovider.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_defaultconnectionprovider.s -S u/t_net_http_defaultconnectionprovider.cpp

u/t_net_http_dispatcher.lo: u/t_net_http_dispatcher.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_dispatcher.lo -c u/t_net_http_dispatcher.cpp

u/t_net_http_dispatcher.o: u/t_net_http_dispatcher.cpp
	@echo "        Compiling u/t_net_http_dispatcher.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_dispatcher.o -c u/t_net_http_dispatcher.cpp

u/t_net_http_dispatcher.s: u/t_net_http_dispatcher.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_dispatcher.s -S u/t_net_http_dispatcher.cpp

u/t_net_http_downloadlistener.lo: u/t_net_http_downloadlistener.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_downloadlistener.lo -c u/t_net_http_downloadlistener.cpp

u/t_net_http_downloadlistener.o: u/t_net_http_downloadlistener.cpp
	@echo "        Compiling u/t_net_http_downloadlistener.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_downloadlistener.o -c u/t_net_http_downloadlistener.cpp

u/t_net_http_downloadlistener.s: u/t_net_http_downloadlistener.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_downloadlistener.s -S u/t_net_http_downloadlistener.cpp

u/t_net_http_errorresponse.lo: u/t_net_http_errorresponse.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_errorresponse.lo -c u/t_net_http_errorresponse.cpp

u/t_net_http_errorresponse.o: u/t_net_http_errorresponse.cpp
	@echo "        Compiling u/t_net_http_errorresponse.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_errorresponse.o -c u/t_net_http_errorresponse.cpp

u/t_net_http_errorresponse.s: u/t_net_http_errorresponse.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_errorresponse.s -S u/t_net_http_errorresponse.cpp

u/t_net_http_formparser.lo: u/t_net_http_formparser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_formparser.lo -c u/t_net_http_formparser.cpp

u/t_net_http_formparser.o: u/t_net_http_formparser.cpp
	@echo "        Compiling u/t_net_http_formparser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_formparser.o -c u/t_net_http_formparser.cpp

u/t_net_http_formparser.s: u/t_net_http_formparser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_formparser.s -S u/t_net_http_formparser.cpp

u/t_net_http_page.lo: u/t_net_http_page.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_page.lo -c u/t_net_http_page.cpp

u/t_net_http_page.o: u/t_net_http_page.cpp
	@echo "        Compiling u/t_net_http_page.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_page.o -c u/t_net_http_page.cpp

u/t_net_http_page.s: u/t_net_http_page.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_page.s -S u/t_net_http_page.cpp

u/t_net_http_pagedispatcher.lo: u/t_net_http_pagedispatcher.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagedispatcher.lo -c u/t_net_http_pagedispatcher.cpp

u/t_net_http_pagedispatcher.o: u/t_net_http_pagedispatcher.cpp
	@echo "        Compiling u/t_net_http_pagedispatcher.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagedispatcher.o -c u/t_net_http_pagedispatcher.cpp

u/t_net_http_pagedispatcher.s: u/t_net_http_pagedispatcher.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagedispatcher.s -S u/t_net_http_pagedispatcher.cpp

u/t_net_http_pagerequest.lo: u/t_net_http_pagerequest.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagerequest.lo -c u/t_net_http_pagerequest.cpp

u/t_net_http_pagerequest.o: u/t_net_http_pagerequest.cpp
	@echo "        Compiling u/t_net_http_pagerequest.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagerequest.o -c u/t_net_http_pagerequest.cpp

u/t_net_http_pagerequest.s: u/t_net_http_pagerequest.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pagerequest.s -S u/t_net_http_pagerequest.cpp

u/t_net_http_pageresponse.lo: u/t_net_http_pageresponse.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pageresponse.lo -c u/t_net_http_pageresponse.cpp

u/t_net_http_pageresponse.o: u/t_net_http_pageresponse.cpp
	@echo "        Compiling u/t_net_http_pageresponse.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pageresponse.o -c u/t_net_http_pageresponse.cpp

u/t_net_http_pageresponse.s: u/t_net_http_pageresponse.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_pageresponse.s -S u/t_net_http_pageresponse.cpp

u/t_net_http_request.lo: u/t_net_http_request.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_request.lo -c u/t_net_http_request.cpp

u/t_net_http_request.o: u/t_net_http_request.cpp
	@echo "        Compiling u/t_net_http_request.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_request.o -c u/t_net_http_request.cpp

u/t_net_http_request.s: u/t_net_http_request.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_request.s -S u/t_net_http_request.cpp

u/t_net_http_response.lo: u/t_net_http_response.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_response.lo -c u/t_net_http_response.cpp

u/t_net_http_response.o: u/t_net_http_response.cpp
	@echo "        Compiling u/t_net_http_response.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_response.o -c u/t_net_http_response.cpp

u/t_net_http_response.s: u/t_net_http_response.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_http_response.s -S u/t_net_http_response.cpp

u/t_net_listener.lo: u/t_net_listener.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_listener.lo -c u/t_net_listener.cpp

u/t_net_listener.o: u/t_net_listener.cpp
	@echo "        Compiling u/t_net_listener.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_listener.o -c u/t_net_listener.cpp

u/t_net_listener.s: u/t_net_listener.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_listener.s -S u/t_net_listener.cpp

u/t_net_name.lo: u/t_net_name.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_name.lo -c u/t_net_name.cpp

u/t_net_name.o: u/t_net_name.cpp
	@echo "        Compiling u/t_net_name.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_name.o -c u/t_net_name.cpp

u/t_net_name.s: u/t_net_name.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_name.s -S u/t_net_name.cpp

u/t_net_networkstack.lo: u/t_net_networkstack.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_networkstack.lo -c u/t_net_networkstack.cpp

u/t_net_networkstack.o: u/t_net_networkstack.cpp
	@echo "        Compiling u/t_net_networkstack.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_networkstack.o -c u/t_net_networkstack.cpp

u/t_net_networkstack.s: u/t_net_networkstack.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_networkstack.s -S u/t_net_networkstack.cpp

u/t_net_parameterencoder.lo: u/t_net_parameterencoder.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_parameterencoder.lo -c u/t_net_parameterencoder.cpp

u/t_net_parameterencoder.o: u/t_net_parameterencoder.cpp
	@echo "        Compiling u/t_net_parameterencoder.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_parameterencoder.o -c u/t_net_parameterencoder.cpp

u/t_net_parameterencoder.s: u/t_net_parameterencoder.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_parameterencoder.s -S u/t_net_parameterencoder.cpp

u/t_net_protocolhandler.lo: u/t_net_protocolhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandler.lo -c u/t_net_protocolhandler.cpp

u/t_net_protocolhandler.o: u/t_net_protocolhandler.cpp
	@echo "        Compiling u/t_net_protocolhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandler.o -c u/t_net_protocolhandler.cpp

u/t_net_protocolhandler.s: u/t_net_protocolhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandler.s -S u/t_net_protocolhandler.cpp

u/t_net_protocolhandlerfactory.lo: u/t_net_protocolhandlerfactory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandlerfactory.lo -c u/t_net_protocolhandlerfactory.cpp

u/t_net_protocolhandlerfactory.o: u/t_net_protocolhandlerfactory.cpp
	@echo "        Compiling u/t_net_protocolhandlerfactory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandlerfactory.o -c u/t_net_protocolhandlerfactory.cpp

u/t_net_protocolhandlerfactory.s: u/t_net_protocolhandlerfactory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_protocolhandlerfactory.s -S u/t_net_protocolhandlerfactory.cpp

u/t_net_redis_integerkey.lo: u/t_net_redis_integerkey.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integerkey.lo -c u/t_net_redis_integerkey.cpp

u/t_net_redis_integerkey.o: u/t_net_redis_integerkey.cpp
	@echo "        Compiling u/t_net_redis_integerkey.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integerkey.o -c u/t_net_redis_integerkey.cpp

u/t_net_redis_integerkey.s: u/t_net_redis_integerkey.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integerkey.s -S u/t_net_redis_integerkey.cpp

u/t_net_redis_integersetkey.lo: u/t_net_redis_integersetkey.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integersetkey.lo -c u/t_net_redis_integersetkey.cpp

u/t_net_redis_integersetkey.o: u/t_net_redis_integersetkey.cpp
	@echo "        Compiling u/t_net_redis_integersetkey.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integersetkey.o -c u/t_net_redis_integersetkey.cpp

u/t_net_redis_integersetkey.s: u/t_net_redis_integersetkey.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_integersetkey.s -S u/t_net_redis_integersetkey.cpp

u/t_net_redis_internaldatabase.lo: u/t_net_redis_internaldatabase.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_internaldatabase.lo -c u/t_net_redis_internaldatabase.cpp

u/t_net_redis_internaldatabase.o: u/t_net_redis_internaldatabase.cpp
	@echo "        Compiling u/t_net_redis_internaldatabase.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_internaldatabase.o -c u/t_net_redis_internaldatabase.cpp

u/t_net_redis_internaldatabase.s: u/t_net_redis_internaldatabase.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_internaldatabase.s -S u/t_net_redis_internaldatabase.cpp

u/t_net_redis_key.lo: u/t_net_redis_key.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_key.lo -c u/t_net_redis_key.cpp

u/t_net_redis_key.o: u/t_net_redis_key.cpp
	@echo "        Compiling u/t_net_redis_key.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_key.o -c u/t_net_redis_key.cpp

u/t_net_redis_key.s: u/t_net_redis_key.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_key.s -S u/t_net_redis_key.cpp

u/t_net_redis_sortoperation.lo: u/t_net_redis_sortoperation.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_sortoperation.lo -c u/t_net_redis_sortoperation.cpp

u/t_net_redis_sortoperation.o: u/t_net_redis_sortoperation.cpp
	@echo "        Compiling u/t_net_redis_sortoperation.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_sortoperation.o -c u/t_net_redis_sortoperation.cpp

u/t_net_redis_sortoperation.s: u/t_net_redis_sortoperation.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_sortoperation.s -S u/t_net_redis_sortoperation.cpp

u/t_net_redis_stringkey.lo: u/t_net_redis_stringkey.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringkey.lo -c u/t_net_redis_stringkey.cpp

u/t_net_redis_stringkey.o: u/t_net_redis_stringkey.cpp
	@echo "        Compiling u/t_net_redis_stringkey.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringkey.o -c u/t_net_redis_stringkey.cpp

u/t_net_redis_stringkey.s: u/t_net_redis_stringkey.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringkey.s -S u/t_net_redis_stringkey.cpp

u/t_net_redis_stringsetkey.lo: u/t_net_redis_stringsetkey.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringsetkey.lo -c u/t_net_redis_stringsetkey.cpp

u/t_net_redis_stringsetkey.o: u/t_net_redis_stringsetkey.cpp
	@echo "        Compiling u/t_net_redis_stringsetkey.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringsetkey.o -c u/t_net_redis_stringsetkey.cpp

u/t_net_redis_stringsetkey.s: u/t_net_redis_stringsetkey.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_stringsetkey.s -S u/t_net_redis_stringsetkey.cpp

u/t_net_redis_subtree.lo: u/t_net_redis_subtree.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_subtree.lo -c u/t_net_redis_subtree.cpp

u/t_net_redis_subtree.o: u/t_net_redis_subtree.cpp
	@echo "        Compiling u/t_net_redis_subtree.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_subtree.o -c u/t_net_redis_subtree.cpp

u/t_net_redis_subtree.s: u/t_net_redis_subtree.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_redis_subtree.s -S u/t_net_redis_subtree.cpp

u/t_net_resp_client.lo: u/t_net_resp_client.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_client.lo -c u/t_net_resp_client.cpp

u/t_net_resp_client.o: u/t_net_resp_client.cpp
	@echo "        Compiling u/t_net_resp_client.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_client.o -c u/t_net_resp_client.cpp

u/t_net_resp_client.s: u/t_net_resp_client.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_client.s -S u/t_net_resp_client.cpp

u/t_net_resp_protocolhandler.lo: u/t_net_resp_protocolhandler.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_protocolhandler.lo -c u/t_net_resp_protocolhandler.cpp

u/t_net_resp_protocolhandler.o: u/t_net_resp_protocolhandler.cpp
	@echo "        Compiling u/t_net_resp_protocolhandler.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_protocolhandler.o -c u/t_net_resp_protocolhandler.cpp

u/t_net_resp_protocolhandler.s: u/t_net_resp_protocolhandler.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_resp_protocolhandler.s -S u/t_net_resp_protocolhandler.cpp

u/t_net_securecontext.lo: u/t_net_securecontext.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securecontext.lo -c u/t_net_securecontext.cpp

u/t_net_securecontext.o: u/t_net_securecontext.cpp
	@echo "        Compiling u/t_net_securecontext.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securecontext.o -c u/t_net_securecontext.cpp

u/t_net_securecontext.s: u/t_net_securecontext.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securecontext.s -S u/t_net_securecontext.cpp

u/t_net_securenetworkstack.lo: u/t_net_securenetworkstack.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securenetworkstack.lo -c u/t_net_securenetworkstack.cpp

u/t_net_securenetworkstack.o: u/t_net_securenetworkstack.cpp
	@echo "        Compiling u/t_net_securenetworkstack.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securenetworkstack.o -c u/t_net_securenetworkstack.cpp

u/t_net_securenetworkstack.s: u/t_net_securenetworkstack.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securenetworkstack.s -S u/t_net_securenetworkstack.cpp

u/t_net_securesocket.lo: u/t_net_securesocket.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securesocket.lo -c u/t_net_securesocket.cpp

u/t_net_securesocket.o: u/t_net_securesocket.cpp
	@echo "        Compiling u/t_net_securesocket.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securesocket.o -c u/t_net_securesocket.cpp

u/t_net_securesocket.s: u/t_net_securesocket.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_securesocket.s -S u/t_net_securesocket.cpp

u/t_net_server.lo: u/t_net_server.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_server.lo -c u/t_net_server.cpp

u/t_net_server.o: u/t_net_server.cpp
	@echo "        Compiling u/t_net_server.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_server.o -c u/t_net_server.cpp

u/t_net_server.s: u/t_net_server.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_server.s -S u/t_net_server.cpp

u/t_net_simpleserver.lo: u/t_net_simpleserver.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_simpleserver.lo -c u/t_net_simpleserver.cpp

u/t_net_simpleserver.o: u/t_net_simpleserver.cpp
	@echo "        Compiling u/t_net_simpleserver.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_simpleserver.o -c u/t_net_simpleserver.cpp

u/t_net_simpleserver.s: u/t_net_simpleserver.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_simpleserver.s -S u/t_net_simpleserver.cpp

u/t_net_socket.lo: u/t_net_socket.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_socket.lo -c u/t_net_socket.cpp

u/t_net_socket.o: u/t_net_socket.cpp
	@echo "        Compiling u/t_net_socket.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_socket.o -c u/t_net_socket.cpp

u/t_net_socket.s: u/t_net_socket.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_socket.s -S u/t_net_socket.cpp

u/t_net_url.lo: u/t_net_url.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_url.lo -c u/t_net_url.cpp

u/t_net_url.o: u/t_net_url.cpp
	@echo "        Compiling u/t_net_url.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_url.o -c u/t_net_url.cpp

u/t_net_url.s: u/t_net_url.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_net_url.s -S u/t_net_url.cpp

u/t_string_char.lo: u/t_string_char.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_char.lo -c u/t_string_char.cpp

u/t_string_char.o: u/t_string_char.cpp
	@echo "        Compiling u/t_string_char.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_char.o -c u/t_string_char.cpp

u/t_string_char.s: u/t_string_char.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_char.s -S u/t_string_char.cpp

u/t_string_format.lo: u/t_string_format.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_format.lo -c u/t_string_format.cpp

u/t_string_format.o: u/t_string_format.cpp
	@echo "        Compiling u/t_string_format.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_format.o -c u/t_string_format.cpp

u/t_string_format.s: u/t_string_format.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_format.s -S u/t_string_format.cpp

u/t_string_formatstate.lo: u/t_string_formatstate.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatstate.lo -c u/t_string_formatstate.cpp

u/t_string_formatstate.o: u/t_string_formatstate.cpp
	@echo "        Compiling u/t_string_formatstate.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatstate.o -c u/t_string_formatstate.cpp

u/t_string_formatstate.s: u/t_string_formatstate.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatstate.s -S u/t_string_formatstate.cpp

u/t_string_formattraits.lo: u/t_string_formattraits.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formattraits.lo -c u/t_string_formattraits.cpp

u/t_string_formattraits.o: u/t_string_formattraits.cpp
	@echo "        Compiling u/t_string_formattraits.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formattraits.o -c u/t_string_formattraits.cpp

u/t_string_formattraits.s: u/t_string_formattraits.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formattraits.s -S u/t_string_formattraits.cpp

u/t_string_formatunion.lo: u/t_string_formatunion.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatunion.lo -c u/t_string_formatunion.cpp

u/t_string_formatunion.o: u/t_string_formatunion.cpp
	@echo "        Compiling u/t_string_formatunion.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatunion.o -c u/t_string_formatunion.cpp

u/t_string_formatunion.s: u/t_string_formatunion.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_formatunion.s -S u/t_string_formatunion.cpp

u/t_string_messages.lo: u/t_string_messages.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_messages.lo -c u/t_string_messages.cpp

u/t_string_messages.o: u/t_string_messages.cpp
	@echo "        Compiling u/t_string_messages.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_messages.o -c u/t_string_messages.cpp

u/t_string_messages.s: u/t_string_messages.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_messages.s -S u/t_string_messages.cpp

u/t_string_nulltranslator.lo: u/t_string_nulltranslator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_nulltranslator.lo -c u/t_string_nulltranslator.cpp

u/t_string_nulltranslator.o: u/t_string_nulltranslator.cpp
	@echo "        Compiling u/t_string_nulltranslator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_nulltranslator.o -c u/t_string_nulltranslator.cpp

u/t_string_nulltranslator.s: u/t_string_nulltranslator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_nulltranslator.s -S u/t_string_nulltranslator.cpp

u/t_string_parse.lo: u/t_string_parse.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parse.lo -c u/t_string_parse.cpp

u/t_string_parse.o: u/t_string_parse.cpp
	@echo "        Compiling u/t_string_parse.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parse.o -c u/t_string_parse.cpp

u/t_string_parse.s: u/t_string_parse.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parse.s -S u/t_string_parse.cpp

u/t_string_parsememory.lo: u/t_string_parsememory.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parsememory.lo -c u/t_string_parsememory.cpp

u/t_string_parsememory.o: u/t_string_parsememory.cpp
	@echo "        Compiling u/t_string_parsememory.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parsememory.o -c u/t_string_parsememory.cpp

u/t_string_parsememory.s: u/t_string_parsememory.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_parsememory.s -S u/t_string_parsememory.cpp

u/t_string_posixfilenames.lo: u/t_string_posixfilenames.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_posixfilenames.lo -c u/t_string_posixfilenames.cpp

u/t_string_posixfilenames.o: u/t_string_posixfilenames.cpp
	@echo "        Compiling u/t_string_posixfilenames.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_posixfilenames.o -c u/t_string_posixfilenames.cpp

u/t_string_posixfilenames.s: u/t_string_posixfilenames.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_posixfilenames.s -S u/t_string_posixfilenames.cpp

u/t_string_proxytranslator.lo: u/t_string_proxytranslator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_proxytranslator.lo -c u/t_string_proxytranslator.cpp

u/t_string_proxytranslator.o: u/t_string_proxytranslator.cpp
	@echo "        Compiling u/t_string_proxytranslator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_proxytranslator.o -c u/t_string_proxytranslator.cpp

u/t_string_proxytranslator.s: u/t_string_proxytranslator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_proxytranslator.s -S u/t_string_proxytranslator.cpp

u/t_string_string.lo: u/t_string_string.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_string.lo -c u/t_string_string.cpp

u/t_string_string.o: u/t_string_string.cpp
	@echo "        Compiling u/t_string_string.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_string.o -c u/t_string_string.cpp

u/t_string_string.s: u/t_string_string.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_string.s -S u/t_string_string.cpp

u/t_string_translator.lo: u/t_string_translator.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_translator.lo -c u/t_string_translator.cpp

u/t_string_translator.o: u/t_string_translator.cpp
	@echo "        Compiling u/t_string_translator.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_translator.o -c u/t_string_translator.cpp

u/t_string_translator.s: u/t_string_translator.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_string_translator.s -S u/t_string_translator.cpp

u/t_sys_atomicinteger.lo: u/t_sys_atomicinteger.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_atomicinteger.lo -c u/t_sys_atomicinteger.cpp

u/t_sys_atomicinteger.o: u/t_sys_atomicinteger.cpp
	@echo "        Compiling u/t_sys_atomicinteger.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_atomicinteger.o -c u/t_sys_atomicinteger.cpp

u/t_sys_atomicinteger.s: u/t_sys_atomicinteger.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_atomicinteger.s -S u/t_sys_atomicinteger.cpp

u/t_sys_commandlineparser.lo: u/t_sys_commandlineparser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_commandlineparser.lo -c u/t_sys_commandlineparser.cpp

u/t_sys_commandlineparser.o: u/t_sys_commandlineparser.cpp
	@echo "        Compiling u/t_sys_commandlineparser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_commandlineparser.o -c u/t_sys_commandlineparser.cpp

u/t_sys_commandlineparser.s: u/t_sys_commandlineparser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_commandlineparser.s -S u/t_sys_commandlineparser.cpp

u/t_sys_dialog.lo: u/t_sys_dialog.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_dialog.lo -c u/t_sys_dialog.cpp

u/t_sys_dialog.o: u/t_sys_dialog.cpp
	@echo "        Compiling u/t_sys_dialog.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_dialog.o -c u/t_sys_dialog.cpp

u/t_sys_dialog.s: u/t_sys_dialog.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_dialog.s -S u/t_sys_dialog.cpp

u/t_sys_duration.lo: u/t_sys_duration.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_duration.lo -c u/t_sys_duration.cpp

u/t_sys_duration.o: u/t_sys_duration.cpp
	@echo "        Compiling u/t_sys_duration.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_duration.o -c u/t_sys_duration.cpp

u/t_sys_duration.s: u/t_sys_duration.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_duration.s -S u/t_sys_duration.cpp

u/t_sys_environment.lo: u/t_sys_environment.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_environment.lo -c u/t_sys_environment.cpp

u/t_sys_environment.o: u/t_sys_environment.cpp
	@echo "        Compiling u/t_sys_environment.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_environment.o -c u/t_sys_environment.cpp

u/t_sys_environment.s: u/t_sys_environment.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_environment.s -S u/t_sys_environment.cpp

u/t_sys_error.lo: u/t_sys_error.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_error.lo -c u/t_sys_error.cpp

u/t_sys_error.o: u/t_sys_error.cpp
	@echo "        Compiling u/t_sys_error.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_error.o -c u/t_sys_error.cpp

u/t_sys_error.s: u/t_sys_error.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_error.s -S u/t_sys_error.cpp

u/t_sys_log.lo: u/t_sys_log.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_log.lo -c u/t_sys_log.cpp

u/t_sys_log.o: u/t_sys_log.cpp
	@echo "        Compiling u/t_sys_log.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_log.o -c u/t_sys_log.cpp

u/t_sys_log.s: u/t_sys_log.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_log.s -S u/t_sys_log.cpp

u/t_sys_loglistener.lo: u/t_sys_loglistener.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_loglistener.lo -c u/t_sys_loglistener.cpp

u/t_sys_loglistener.o: u/t_sys_loglistener.cpp
	@echo "        Compiling u/t_sys_loglistener.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_loglistener.o -c u/t_sys_loglistener.cpp

u/t_sys_loglistener.s: u/t_sys_loglistener.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_loglistener.s -S u/t_sys_loglistener.cpp

u/t_sys_longcommandlineparser.lo: u/t_sys_longcommandlineparser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_longcommandlineparser.lo -c u/t_sys_longcommandlineparser.cpp

u/t_sys_longcommandlineparser.o: u/t_sys_longcommandlineparser.cpp
	@echo "        Compiling u/t_sys_longcommandlineparser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_longcommandlineparser.o -c u/t_sys_longcommandlineparser.cpp

u/t_sys_longcommandlineparser.s: u/t_sys_longcommandlineparser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_longcommandlineparser.s -S u/t_sys_longcommandlineparser.cpp

u/t_sys_mutex.lo: u/t_sys_mutex.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutex.lo -c u/t_sys_mutex.cpp

u/t_sys_mutex.o: u/t_sys_mutex.cpp
	@echo "        Compiling u/t_sys_mutex.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutex.o -c u/t_sys_mutex.cpp

u/t_sys_mutex.s: u/t_sys_mutex.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutex.s -S u/t_sys_mutex.cpp

u/t_sys_mutexguard.lo: u/t_sys_mutexguard.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutexguard.lo -c u/t_sys_mutexguard.cpp

u/t_sys_mutexguard.o: u/t_sys_mutexguard.cpp
	@echo "        Compiling u/t_sys_mutexguard.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutexguard.o -c u/t_sys_mutexguard.cpp

u/t_sys_mutexguard.s: u/t_sys_mutexguard.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_mutexguard.s -S u/t_sys_mutexguard.cpp

u/t_sys_parsedtime.lo: u/t_sys_parsedtime.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_parsedtime.lo -c u/t_sys_parsedtime.cpp

u/t_sys_parsedtime.o: u/t_sys_parsedtime.cpp
	@echo "        Compiling u/t_sys_parsedtime.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_parsedtime.o -c u/t_sys_parsedtime.cpp

u/t_sys_parsedtime.s: u/t_sys_parsedtime.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_parsedtime.s -S u/t_sys_parsedtime.cpp

u/t_sys_semaphore.lo: u/t_sys_semaphore.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_semaphore.lo -c u/t_sys_semaphore.cpp

u/t_sys_semaphore.o: u/t_sys_semaphore.cpp
	@echo "        Compiling u/t_sys_semaphore.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_semaphore.o -c u/t_sys_semaphore.cpp

u/t_sys_semaphore.s: u/t_sys_semaphore.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_semaphore.s -S u/t_sys_semaphore.cpp

u/t_sys_standardcommandlineparser.lo: u/t_sys_standardcommandlineparser.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_standardcommandlineparser.lo -c u/t_sys_standardcommandlineparser.cpp

u/t_sys_standardcommandlineparser.o: u/t_sys_standardcommandlineparser.cpp
	@echo "        Compiling u/t_sys_standardcommandlineparser.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_standardcommandlineparser.o -c u/t_sys_standardcommandlineparser.cpp

u/t_sys_standardcommandlineparser.s: u/t_sys_standardcommandlineparser.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_standardcommandlineparser.s -S u/t_sys_standardcommandlineparser.cpp

u/t_sys_thread.lo: u/t_sys_thread.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_thread.lo -c u/t_sys_thread.cpp

u/t_sys_thread.o: u/t_sys_thread.cpp
	@echo "        Compiling u/t_sys_thread.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_thread.o -c u/t_sys_thread.cpp

u/t_sys_thread.s: u/t_sys_thread.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_thread.s -S u/t_sys_thread.cpp

u/t_sys_time.lo: u/t_sys_time.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_time.lo -c u/t_sys_time.cpp

u/t_sys_time.o: u/t_sys_time.cpp
	@echo "        Compiling u/t_sys_time.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_time.o -c u/t_sys_time.cpp

u/t_sys_time.s: u/t_sys_time.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_time.s -S u/t_sys_time.cpp

u/t_sys_types.lo: u/t_sys_types.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_types.lo -c u/t_sys_types.cpp

u/t_sys_types.o: u/t_sys_types.cpp
	@echo "        Compiling u/t_sys_types.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_types.o -c u/t_sys_types.cpp

u/t_sys_types.s: u/t_sys_types.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_sys_types.s -S u/t_sys_types.cpp

u/t_tmp_copycv.lo: u/t_tmp_copycv.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_copycv.lo -c u/t_tmp_copycv.cpp

u/t_tmp_copycv.o: u/t_tmp_copycv.cpp
	@echo "        Compiling u/t_tmp_copycv.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_copycv.o -c u/t_tmp_copycv.cpp

u/t_tmp_copycv.s: u/t_tmp_copycv.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_copycv.s -S u/t_tmp_copycv.cpp

u/t_tmp_dereferencetype.lo: u/t_tmp_dereferencetype.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_dereferencetype.lo -c u/t_tmp_dereferencetype.cpp

u/t_tmp_dereferencetype.o: u/t_tmp_dereferencetype.cpp
	@echo "        Compiling u/t_tmp_dereferencetype.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_dereferencetype.o -c u/t_tmp_dereferencetype.cpp

u/t_tmp_dereferencetype.s: u/t_tmp_dereferencetype.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_dereferencetype.s -S u/t_tmp_dereferencetype.cpp

u/t_tmp_ifthenelse.lo: u/t_tmp_ifthenelse.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_ifthenelse.lo -c u/t_tmp_ifthenelse.cpp

u/t_tmp_ifthenelse.o: u/t_tmp_ifthenelse.cpp
	@echo "        Compiling u/t_tmp_ifthenelse.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_ifthenelse.o -c u/t_tmp_ifthenelse.cpp

u/t_tmp_ifthenelse.s: u/t_tmp_ifthenelse.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_ifthenelse.s -S u/t_tmp_ifthenelse.cpp

u/t_tmp_issametype.lo: u/t_tmp_issametype.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_issametype.lo -c u/t_tmp_issametype.cpp

u/t_tmp_issametype.o: u/t_tmp_issametype.cpp
	@echo "        Compiling u/t_tmp_issametype.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_issametype.o -c u/t_tmp_issametype.cpp

u/t_tmp_issametype.s: u/t_tmp_issametype.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_issametype.s -S u/t_tmp_issametype.cpp

u/t_tmp_sizedint.lo: u/t_tmp_sizedint.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizedint.lo -c u/t_tmp_sizedint.cpp

u/t_tmp_sizedint.o: u/t_tmp_sizedint.cpp
	@echo "        Compiling u/t_tmp_sizedint.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizedint.o -c u/t_tmp_sizedint.cpp

u/t_tmp_sizedint.s: u/t_tmp_sizedint.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizedint.s -S u/t_tmp_sizedint.cpp

u/t_tmp_sizeduint.lo: u/t_tmp_sizeduint.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizeduint.lo -c u/t_tmp_sizeduint.cpp

u/t_tmp_sizeduint.o: u/t_tmp_sizeduint.cpp
	@echo "        Compiling u/t_tmp_sizeduint.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizeduint.o -c u/t_tmp_sizeduint.cpp

u/t_tmp_sizeduint.s: u/t_tmp_sizeduint.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_sizeduint.s -S u/t_tmp_sizeduint.cpp

u/t_tmp_stripcv.lo: u/t_tmp_stripcv.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_stripcv.lo -c u/t_tmp_stripcv.cpp

u/t_tmp_stripcv.o: u/t_tmp_stripcv.cpp
	@echo "        Compiling u/t_tmp_stripcv.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_stripcv.o -c u/t_tmp_stripcv.cpp

u/t_tmp_stripcv.s: u/t_tmp_stripcv.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_stripcv.s -S u/t_tmp_stripcv.cpp

u/t_tmp_typelist.lo: u/t_tmp_typelist.cpp
	$(CXX) -fPIC $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_typelist.lo -c u/t_tmp_typelist.cpp

u/t_tmp_typelist.o: u/t_tmp_typelist.cpp
	@echo "        Compiling u/t_tmp_typelist.cpp..."
	@$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_typelist.o -c u/t_tmp_typelist.cpp

u/t_tmp_typelist.s: u/t_tmp_typelist.cpp
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -D_CXXTEST_HAVE_EH -D_CXXTEST_HAVE_STD -g -o u/t_tmp_typelist.s -S u/t_tmp_typelist.cpp


include depend.mk

.cpp.lo:
	$(CXX) -fPIC $(CXXFLAGS) -o $@ -c $<

.cpp.o:
	@echo "        Compiling $<..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

.cpp.s:
	$(CXX) $(CXXFLAGS) -o $@ -S $<


