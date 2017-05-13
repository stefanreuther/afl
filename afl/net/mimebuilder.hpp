/**
  *  \file afl/net/mimebuilder.hpp
  */
#ifndef AFL_AFL_NET_MIMEBUILDER_HPP
#define AFL_AFL_NET_MIMEBUILDER_HPP

#include <list>
#include "afl/string/string.hpp"
#include "afl/io/datasink.hpp"

namespace afl { namespace net {

    /** Simple MIME Multipart Entity Builder.
        This gathers up a MIME message line by line, and fills in the boundaries.

        Usage:
        - construct an object <tt>MimeBuilder b("content-type");</tt> giving it the multipart content type as parameter
        - use addHeader(...) to add additional headers
        - use addBoundary() followed by multiple addHeader(), followed by addLine()/addBase64()/addLineQP() to add parts
        - use addBoundary() to add the final boundary
        - use finish() to finish the object
        - possibly use removeInitialHeaders()
        - use computeSize() and write() to get the data

        Content-Transfer-Encoding is not managed by MimeBuilder,
        you must write a Content-Transfer-Encoding line that matches your addXxx() calls yourself. */
    class MimeBuilder {
     public:
        /** Constructor.
            \param ctype Content type. Can be "multipart/mixed", "multipart/form-data", etc.
            Can be blank if you wish a non-multipart content type and want to add the
            header yourself. */
        explicit MimeBuilder(String_t ctype);

        /** Destructor. */
        ~MimeBuilder();

        /** Add header to message. Currently does no postprocessing of the header data.
            \param name   Name of header
            \param value  Value of header */
        void addHeader(String_t name, String_t value);

        /** Add line to message.
            Does not modify (process) the line in any way.
            \param line Line to add */
        void addLine(String_t line);

        /** Add base64 data.
            \param data Data to add */
        void addBase64(afl::base::ConstBytes_t data);

        /** Add quoted-printable line.
            Does QP encoding before adding the line to the message.
            \param line [in] The line to add. */
        void addLineQP(String_t line);

        /** Add raw data.
            \param data Data to add */
        void addRawData(afl::base::ConstBytes_t data);

        /** Add a boundary.
            After this call, you can either write a new part using addHeader() etc., or use finish(). */
        void addBoundary();

        /** Complete the MIME entity.
            This figures out the boundary string to use, and fills it in at appropriate places. */
        void finish();

        /** Compute total size of this entity. */
        size_t getTotalSize() const;

        /** Write this entity onto a stream. */
        void write(afl::io::DataSink& out, bool escapeDots);

        /** Removes everything before the first boundary.
            Effectively, that's all the headers plus the blank line separating headers from body.
            \return Removed text */
        String_t removeInitialHeaders();

     private:
        // FIXME: member names
        /** Type of an element. */
        enum ElementType {
            TextElement,              /**< Raw text. */
            ContentTypeElement,       /**< Content-type line. */
            BoundaryElement           /**< Boundary. */
        };
        /** Element (line) of our message. */
        struct Element {
            String_t m_Text;
            ElementType m_Type;

            Element(const String_t& text, ElementType type)
                : m_Text(text), m_Type(type) { }
        };
        /** Content of our message. */
        std::list<Element> m_Content;

        /** Content-Type to use. */
        String_t m_ContentType;

        /** Accumulator for building base64 parts. */
        String_t m_Base64Accumulator;

        /** Set if we have a pending CRLF after binary data. */
        bool m_PendingCRLF;

        /** State. */
        enum State {
            StateHeader,
            StateBody
        };
        State m_State;

        void flushHeaderAndStartBody();
        void flushBase64();
    };

} }

#endif
