/**
  *  \file afl/checksums/hash.hpp
  *  \brief Class afl::checksums::Hash
  */
#ifndef AFL_AFL_CHECKSUMS_HASH_HPP
#define AFL_AFL_CHECKSUMS_HASH_HPP

#include "afl/base/deletable.hpp"
#include "afl/base/memory.hpp"
#include "afl/base/types.hpp"
#include "afl/string/string.hpp"

namespace afl { namespace checksums {

    /** Base class for a cryptographic hash.
        Cryptographic hashes have a few more bits than checksums
        and therefore require a more elaborate interface than the Checksum class.

        To compute a hash:
        - call add() repeatedly to add bytes to the computation
        - call getHash() to get the hash value
        - call clear() to reset the object and start computing a new hash

        You can call getHash() multiple times to obtain intermediate results. */
    class Hash : public afl::base::Deletable {
     public:
        /*
         *  Types
         */

        /** Type of input data. Shortcut to simplify code. */
        typedef afl::base::ConstBytes_t ConstBytes_t;

        /** Type of output data. Shortcut to simplify code. */
        typedef afl::base::Bytes_t Bytes_t;

        /** Maximum size.
            This is the maximum size of hashes that clients (including Hash's method)
            are required to support.
            \see getHashSize(). */
        static const size_t MAX_HASH_SIZE = 64;

        /** Maximum block size.
            This is the maximum block size that that clients (including Hash's method)
            are required to support.
            \see getBlockSize(). */
        static const size_t MAX_BLOCK_SIZE = 128;


        /*
         *  Virtuals
         */

        /** Reset hash.
            Starts computing a new hash. */
        virtual void clear() = 0;

        /** Add data to the hash.
            Hashes must accept arbitrary byte streams (i.e. no blocking requirements).

            Invariant: adding bytes to a hash produces the same result
            no matter how the bytes are split into individual add() calls.
            That is, adding 100 bytes as one block or as 100 single bytes should produce the same result.

            \param data Data to add */
        virtual void add(ConstBytes_t data) = 0;

        /** Get size of hash in bytes.
            This is the number of bytes that will be returned by getHash() and which,
            in turn, the caller must provide to receive the whole hash.

            This method should return a constant value for each class instance
            (i.e. not data-dependant).

            Although the interface can support arbitrarily large hashes,
            which could be useful for certain usecases,
            regular users do not expect more than MAX_HASH_SIZE.
            \return number of bytes, must not be 0 */
        virtual size_t getHashSize() const = 0;

        /** Get block size of this hash in bytes.
            This number is required for HMAC.

            This method should return a constant value for each class instance
            (i.e. not data-dependant).

            \return number of bytes, must not be 0 */
        virtual size_t getBlockSize() const = 0;

        /** Get hash.
            Can be called at any time to obtain the current hash.
            However, this might be an expensive operation ("more than just memcpy").

            If the hash is larger than the provided buffer, it is truncated.
            If the hash is smaller than the buffer, only that initial part will be initialized.

            Invariant: calling this method twice without intervening clear() or add()
            will return the same data.

            \param data Buffer for the hash, should be getHashSize() bytes or more.
            \return Buffer containing the hash (a subrange of %data, possibly truncated). */
        virtual Bytes_t getHash(Bytes_t data) const = 0;


        /*
         *  Concrete methods
         */

        /** Compute HMAC (Hash-based Message Authentication Code).
            This is a symmetric key-based message authentication code
            that avoids a few cryptographic attacks against a "naked" hash.

            This method prepares the HMAC hash inside this object.
            Use getHash() to obtain the data.

            HMAC only works for hashes with up to MAX_SIZE (64 bytes / 512 bits).

            Unlike regular hashing that allows multiple calls to add(),
            computeHMAC() needs key and data all at once.

            Specification reference: RFC 2104.

            \param key Secret key
            \param data Data to sign

            \throw afl::except::UnsupportedException if getHashSize() > MAX_SIZE */
        void computeHMAC(ConstBytes_t key, ConstBytes_t data);

        /** Compute PBKDF2 (Password-Based Key Derivation Function Two).
            This is a standardized generic mechanism to create a password hash or key from password and salt.
            It uses the current hash's HMAC (computeHMAC()) as pseudo-random function.
            It can generate arbitrarily large password hashes ((2**32-1) * getHashSize(), actually).

            Specification reference: RFC 2898, RFC 6070.

            \param out [out] Output data generated here. This buffer will be completely filled,
            independant from the hash's parameters.
            \param password [in] Password.
            \param salt [in] Salt. This implementation supports at most MAX_BLOCK_SIZE bytes in a salt.
            \param numIterations [in] Number of iterations, system parameter.

            \todo PBKDF2 would allow larger block sizes than MAX_BLOCK_SIZE.
            One way around that limitation would be to regularily hash salt/iteration if they get too large,
            because that's what HMAC will do anyway. */
        void computePBKDF2(Bytes_t out, ConstBytes_t password, ConstBytes_t salt, uint32_t numIterations);

        /** Get hash as hex string.
            This format is conventionally used to pass hashes around.
            \see getHash()
            \return hash as hex string */
        String_t getHashAsHexString() const;
    };

} }

#endif
