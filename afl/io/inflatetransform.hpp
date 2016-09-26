/**
  *  \file afl/io/inflatetransform.hpp
  *  \brief Class afl::io::InflateTransform
  */
#ifndef AFL_AFL_IO_INFLATETRANSFORM_HPP
#define AFL_AFL_IO_INFLATETRANSFORM_HPP

#include <memory>
#include "afl/io/transform.hpp"

namespace afl { namespace io {

    /** Inflate transformation.
        This transformation inflates data (using zlib).

        <b>Conditional availability:</b>
        If zlib was not available when building afl, this class will refuse being constructed
        by throwing an afl::except::UnsupportedException.
        You can use isAvailable() to check availability beforehand. */
    class InflateTransform : public Transform {
     public:
        /** Personality (stream type). */
        enum Personality {
            Gzip,               ///< Gzip file (with header).
            Raw                 ///< Just a deflate stream.
        };

        /** Constructor.
            \param personality Personality (stream type) */
        explicit InflateTransform(Personality personality);

        /** Destructor. */
        ~InflateTransform();

        // Transform:
        virtual void transform(afl::base::ConstBytes_t& in, afl::base::Bytes_t& out);
        virtual void flush();

        /** Check availability.
            \retval true zlib functionality is available; you can construct InflateDataSink objects
            \retval false zlib functionality is not available; constructing InflateDataSink will fail */
        static bool isAvailable();

     private:
        class Impl;
        std::auto_ptr<Impl> m_pImpl;
    };

} }

#endif
