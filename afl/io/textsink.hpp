/**
  *  \file afl/io/textsink.hpp
  *  \brief Class afl::io::TextSink
  */
#ifndef AFL_AFL_IO_TEXTSINK_HPP
#define AFL_AFL_IO_TEXTSINK_HPP

#include "afl/string/string.hpp"

namespace afl { namespace io {

    class DataSink;

    /** Text front-end for DataSink.
        This class provides a facade that makes dealing with a DataSink easier for the common case of producing textual data.

        TextSink assumes that you produce textual data in the same encoding as stored in the strings (i.e. UTF-8).
        No character set translation will happen.

        TextSink accepts "const char*" arguments and treats these as null-terminated strings.
        In contrast, the ConstStringMemory_t signature would implicitly include the null byte. */
    class TextSink {
     public:
        /** Constructor.
            \param sink DataSink, must out-live the TextSink */
        explicit TextSink(DataSink& sink);

        /** Write null-terminated string.
            \param str String
            \return *this */
        TextSink& operator<<(const char* str);

        /** Write string.
            \param str String
            \return *this */
        TextSink& operator<<(const String_t& str);

        /** Write text buffer.
            \param mem Text buffer
            \return *this */
        TextSink& operator<<(afl::string::ConstStringMemory_t mem);

     private:
        DataSink& m_sink;
    };

} }

#endif
