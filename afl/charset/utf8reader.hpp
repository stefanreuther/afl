/**
  *  \file afl/charset/utf8reader.hpp
  *  \brief Class afl::charset::Utf8Reader
  */
#ifndef AFL_AFL_CHARSET_UTF8READER_HPP
#define AFL_AFL_CHARSET_UTF8READER_HPP

#include "afl/base/memory.hpp"
#include "afl/charset/unicode.hpp"

namespace afl { namespace charset {

    /** Sequential Access to UTF-8 String. */
    class Utf8Reader {
     public:
        /** Constructor.
            \param data Byte stream to read
            \param flags Options; combination of the AllowXxx values. */
        Utf8Reader(afl::base::ConstBytes_t data, uint32_t flags);

        /** Read a character.
            \pre hasMore
            \return character */
        Unichar_t eat();

        /** Check whether there are more characters.
            \return true iff the input sequence has more characters */
        bool hasMore() const;

        /** Count number of remaining characters.
            \return the number of times eat() can be called */
        size_t count() const;

        /** Get remaining unparsed sequence.
            \return Handle to unread data */
        afl::base::ConstBytes_t getRemainder() const;

     private:
        afl::base::ConstBytes_t m_data;

        uint32_t m_flags;

        struct Character;

        void parse(afl::base::ConstBytes_t data, Character& result) const;
    };

} }

#endif
