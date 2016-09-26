/**
  *  \file afl/io/xml/entityhandler.hpp
  *  \brief Interface afl::io::xml::EntityHandler
  */
#ifndef AFL_AFL_IO_XML_ENTITYHANDLER_HPP
#define AFL_AFL_IO_XML_ENTITYHANDLER_HPP

#include "afl/base/deletable.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace io { namespace xml {

    /** XML Entity handler.
        This interface allows an XML reader (BaseReader) to expand character references (&amp;xxx;). */
    class EntityHandler : public afl::base::Deletable {
     public:
        /** Expand entity reference.
            This function receives the entity name (text between "&" and ";") and must return a replacement.
            There is no builtin magic in the XML readers;
            this function is responsible for all entity references including numeric ones ("&amp;#12345;").
            \param name Entity name
            \return replacement text, UTF-8 */
        virtual String_t expandEntityReference(String_t name) = 0;
    };

} } }

#endif
