/**
  *  \file afl/io/transformdatasink.hpp
  *  \brief Class afl::io::TransformDataSink
  */
#ifndef AFL_AFL_IO_TRANSFORMDATASINK_HPP
#define AFL_AFL_IO_TRANSFORMDATASINK_HPP

#include <memory>
#include "afl/io/datasink.hpp"

namespace afl { namespace io {

    class Transform;

    /** Data sink using a transformation.
        Processes all provided data using a Transform and pushes it into another DataSink. */
    class TransformDataSink : public DataSink {
     public:
        /** Constructor.
            This constructs the TransformDataSink without the transformation.
            The transformation must be set explicitly.
            Until a transformation is set, data is passed through unmodified.

            \param other Data sink that receives the transformed data. Lifetime must exceed that of the TransformDataSink. */
        TransformDataSink(DataSink& other);

        /** Destructor. */
        virtual ~TransformDataSink();

        /** Set transformation.
            \param pTransform Transformation to use. TransformDataSink assumes ownership. */
        void setNewTransform(Transform* pTransform);

        // DataSink:
        virtual bool handleData(const String_t& name, afl::base::ConstBytes_t& data);

     private:
        /** Transformation. Can be null. */
        std::auto_ptr<Transform> m_pTransform;

        /** Other sink. */
        DataSink& m_other;
    };

} }

#endif
