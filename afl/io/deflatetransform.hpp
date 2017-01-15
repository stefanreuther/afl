/**
  *  \file afl/io/deflatetransform.hpp
  *  \brief Class afl::io::DeflateTransform
  */
#ifndef AFL_AFL_IO_DEFLATETRANSFORM_HPP
#define AFL_AFL_IO_DEFLATETRANSFORM_HPP

#include <memory>
#include "afl/io/transform.hpp"

namespace afl { namespace io {

    /** Deflate transformation.
        This transformation deflates (compresses) data (using zlib).

        <b>Conditional availability:</b>
        If zlib was not available when building afl, this class will refuse being constructed
        by throwing an afl::except::UnsupportedException.
        You can use isAvailable() to check availability beforehand. */
    class DeflateTransform : public Transform {
     public:
        /** Personality (stream type). */
        enum Personality {
            Gzip,               ///< Gzip file (with header).
            Raw,                ///< Just a deflate stream.
            Zlib                ///< Including Zlib header.
        };

        /** Constructor.
            \param personality Personality (stream type) */
        explicit DeflateTransform(Personality personality);

        /** Destructor. */
        ~DeflateTransform();

        // Transform:
        virtual void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out);
        virtual void flush();

        /** Check availability.
            \retval true zlib functionality is available; you can construct DeflateDataSink objects
            \retval false zlib functionality is not available; constructing DeflateDataSink will fail */
        static bool isAvailable();

     private:
        class Impl;
        std::auto_ptr<Impl> m_pImpl;
    };

} }

#endif
