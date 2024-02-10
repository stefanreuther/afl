/**
  *  \file afl/io/inflatedatasink.hpp
  *  \brief Class afl::io::InflateDataSink
  */
#ifndef AFL_AFL_IO_INFLATEDATASINK_HPP
#define AFL_AFL_IO_INFLATEDATASINK_HPP

#include <memory>
#include "afl/io/inflatetransform.hpp"
#include "afl/io/transformdatasink.hpp"

namespace afl { namespace io {

    /** Inflate data sink.
        This data sink inflates data (using zlib) and pushes the deflated data into another data sink.
        This is a convenience class that combines an InflateTransform and a TransformDataSink.

        <b>Conditional availability:</b>
        If zlib was not available when building afl, this class will refuse being constructed
        by throwing an afl::except::UnsupportedException.
        You can use isAvailable() to check availability beforehand. */
    class InflateDataSink : public TransformDataSink {
     public:
        /** Personality (stream type). */
        typedef InflateTransform::Personality Personality;

        /** Gzip file (with header). */
        static const Personality Gzip = InflateTransform::Gzip;

        /** Just a deflate stream. */
        static const Personality Raw = InflateTransform::Raw;

        /** Constructor.
            \param peer Target data sink. Lifetime must exceed that of the InflateDataSink.
            \param personality Personality (stream type) */
        InflateDataSink(DataSink& peer, Personality personality);

        /** Descriptor. */
        ~InflateDataSink();

        /** Check availability.
            \retval true zlib functionality is available; you can construct InflateDataSink objects
            \retval false zlib functionality is not available; constructing InflateDataSink will fail */
        static bool isAvailable();
    };

} }

#endif
