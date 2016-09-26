/**
  *  \file afl/net/redis/sortoperation.hpp
  *  \brief Class afl::net::redis::SortOperation
  */
#ifndef AFL_AFL_NET_REDIS_SORTOPERATION_HPP
#define AFL_AFL_NET_REDIS_SORTOPERATION_HPP

#include "afl/base/ptr.hpp"
#include "afl/data/integerlist.hpp"
#include "afl/data/segment.hpp"
#include "afl/data/stringlist.hpp"
#include "afl/data/value.hpp"
#include "afl/net/commandhandler.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace net { namespace redis {

    class Key;
    class ListKey;
    class Field;

    /** Sort operation.
        Allows to build a sort operation to obtain the values of a ListKey or SetKey in a sorted fashion.

        Definitions/features:
        - <em>origin key</em>: the original ListKey or SetKey
        - <em>origin value</em>: the value from the origin key
        - <em>sort key</em>: the value used to determine the sorting.
          By default, this is the origin value.
          Use by() to define a different sort key (from a different database key).
        - <em>sort order</em>: by default, the result is sorted numerically.
          Use the sort() functions to change that.
        - By default, the command returns the origin values.
          Use the get() functions to define different values.
          If you call three get() functions, the operation will produce a list containing 3 times the number of values as the origin key,
          with the 3 values corresponding to one origin value in sequence. */
    class SortOperation {
     public:
        /** Constructor.
            Normally, you don't call the SortOperation constructor directly,
            but use ListKey::sort or SetKey::sort instead.
            \param key Origin key. Must be a ListKey or SetKey. */
        SortOperation(const Key& key);

        /** Destructor. */
        ~SortOperation();

        /** Define sort key.
            Call at most once.
            Default is to sort by the origin value.
            The pattern must contain one "*" that is replaced by the origin value,
            to produce the name of a key or hash field providing the sort key.
            \param pattern Pattern
            \return *this */
        SortOperation& by(String_t pattern);

        /** Define sort key, given a key template.
            Call at most once.
            Default is to sort by the origin value.
            This key name must contain one "*" that is replaced by the origin value,
            to produce the actual name of the key providing the sort key.
            \param key Sort pattern
            \return *this */
        SortOperation& by(const Key& key);

        /** Define sort key, given a hash field template.
            Call at most once.
            Default is to sort by the origin value.
            This key name must contain one "*" that is replaced by the origin value,
            to produce the actual name of the key and field providing the sort key.
            \param field Sort pattern
            \return *this */
        SortOperation& by(const Field& field);

        /** Request origin value return.
            Call at most once.
            This requests that the result list contains the origin value.
            By default, if none of the get() functions has been used,
            this operation behaves as if a single call to get() happened.
            \return *this */
        SortOperation& get();

        /** Request value return.
            This requests that the result list contain the value of a key or hash field.
            The pattern must contain one "*" that is replaced by the origin value,
            to produce the name of a key or hash field providing the sort key.
            \param pattern Pattern
            \return *this */
        SortOperation& get(String_t pattern);

        /** Request key value return.
            This requests that the result list contain the value of a key.
            This key name must contain one "*" that is replaced by the origin value,
            to produce the name of the key providing the sort key.
            \param key Key
            \return *this */
        SortOperation& get(const Key& key);

        /** Request field value return.
            This requests that the result list contain the value of a hash field.
            This key name must contain one "*" that is replaced by the origin value,
            to produce the name of the key and field providing the sort key.
            \param field Field
            \return *this */
        SortOperation& get(const Field& field);

        /** Limit output.
            Call at most once.
            \param start Number of topmost elements to skip (default=0, return all elements)
            \param count Maximum number of elements to return (default=-1, return all elements)
            \return *this */
        SortOperation& limit(int32_t start, int32_t count);

        /** Enable lexicographical sort.
            Default is numerical.
            Call at most once.
            \return *this */
        SortOperation& sortLexicographical();

        /** Enable reversed (descending) sort.
            Default is ascending.
            Call at most once.
            \return *this */
        SortOperation& sortReversed();

        /** Disable sorting.
            Causes the command to return just the get() values.
            Call at most once.
            \return *this */
        SortOperation& sortDisable();

        /** Get result into a StringList_t.
            This actually executes the operation; call last after all other methods.
            \param out [out] Result appended here */
        void getResult(afl::data::StringList_t& out);

        /** Get result into an IntegerList_t.
            This actually executes the operation; call last after all other methods.
            \param out [out] Result appended here */
        void getResult(afl::data::IntegerList_t& out);

        /** Get raw result.
            This actually executes the operation; call last after all other methods.
            \return newly-allocated value */
        afl::data::Value* getResult();

        /** Store result into a ListKey.
            This actually executes the operation; call last after all other methods.
            \param out Key to store into */
        void storeResult(ListKey out);

     private:
        /// CommandHandler to use.
        CommandHandler& m_ch;

        /// Request. Must be a Ptr<> to make this object copy-constructible.
        afl::base::Ptr<afl::data::Segment> m_request;
    };

} } }

#endif
