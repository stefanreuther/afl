/**
  *  \file u/t_net_securenetworkstack.cpp
  *  \brief Test for afl::net::SecureNetworkStack
  */

#include <cstdlib>
#include "afl/net/securenetworkstack.hpp"

#include "u/t_net.hpp"
#include "afl/async/receiveoperation.hpp"
#include "afl/async/sendoperation.hpp"
#include "afl/base/ptr.hpp"
#include "afl/base/stoppable.hpp"
#include "afl/base/types.hpp"
#include "afl/except/fileproblemexception.hpp"
#include "afl/except/unsupportedexception.hpp"
#include "afl/net/name.hpp"
#include "afl/net/networkstack.hpp"
#include "afl/net/securecontext.hpp"
#include "afl/net/socket.hpp"
#include "afl/sys/thread.hpp"

namespace {
    using afl::net::SecureContext;
    using afl::base::Ptr;
    using afl::base::Ref;
    using afl::net::Name;
    using afl::net::Socket;

    const char PRIVATE_KEY[] =
        "-----BEGIN PRIVATE KEY-----\n"
        "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCg+GuijItRL4uK\n"
        "F6C0bkuYhxHrvWzu0TpcT92eVp+gzEbqs/gXGxh5sJSro4qVioDBlBuef4fdvi1T\n"
        "v3ls4bPxx/OAdQ+FaomkPy60hCjaEPvOn3Es8m+AxLjZxOBybxzJkSRojiYyzEZd\n"
        "70eMXz5AV41PuXQUeID6wevLekXY7SVuOWd9W7SpbHlIVU0cqe0n8CBr5xHfp/nV\n"
        "N900gTVKd4SenIt6ETm4zihxwMMsAKJXkE3VQSo2/5WyKBuMN+azAhBkIhW+HEnx\n"
        "Qs4uDlHxIJhGESKnszAfrkXrq4xuDCTJbCZXQTLha/joVt7XkaH9J+9WeND8PA9v\n"
        "vgpbVo/FAgMBAAECggEAR40E8xT73FOA+Bn2kh0wtxTa72mc4S7ZfSz2+qnQfL4w\n"
        "i6UBkuu1XmoSO5UPRddx0hoOiM5xSyx8o2xKAg71WHSjlSbzhj9L/VkkXpMcQYop\n"
        "E7glq2/8BkXrUMlFZzhimGpOv7bn3c6VPK3epNO1LQ59vsFVir/hvRSzxheMspZK\n"
        "QebDv3Uff5s85EdQetFGKKuAXxqRt403rTNSp98JnZSYfBFVsg7KjSy8x6zieVoQ\n"
        "9rBaXWg7O0hHFel4Am7oLfo91TLZp4/s4pZqLVD6F2U96vKksMwiruV36PP6tSRv\n"
        "OZmrzSTyFtbQjMv2P0YTKrvRuh7IHNqScDIocjN4gQKBgQDWMD/PdTXRutl2ghIg\n"
        "jB8G4kZhtp6wSnmX0s5Fgr0v8CrziAiVzjl+64lTivXLD9mv6WttjO4P6MnT+PGD\n"
        "6dppAXZ85BUmIo0yh4erCdYgOectsSUCmrgbgS9A3qMnq0VQsenSoo1dyAiAa0q4\n"
        "qokQFQ/MfUQHqbgOfZS/sz5M5QKBgQDAZK04rmg+g02FR2PL/kvzuhRHuTzhIQy/\n"
        "5UNXE0mgsNliIPDDCDC6Y5wLBcOqRgfMyPtPFRPDSIdOG7z6N7dS/oO/cc7izxGG\n"
        "ssX3TRKzlHKa7BfiHlsgwUJ9jjc6LHVljHWQPQx9yxtc094Hp0vqnP478nwHHPlL\n"
        "B6a3LNPpYQKBgQCfXhksiH66BbgmuFM6imfPnG9X5ySd8VwVvlGiYMvZrm6W82HH\n"
        "7UDUkfaFBcErDSkg/aHkSlcb2U7K/NDfIRJUzTTXanHeBRlgPn2QouoC4nKDieoK\n"
        "zQot1OpO7xkgvAwAJdDf2/kpjVveCXb4flMrdzBhK9eBiCOeDIywMnMpjQKBgDh+\n"
        "V48JR3rzu1Ojft5rsYnD3iihcAr7VW8Ttw5ej4Qr6ekymv/p68UPcsaZzra1EZ1E\n"
        "dubl3YO+TFzXMEqZCa/JJI6WsJVREGhpHlBbxV4N/QkYwnqXR1L67LdJh1YBsAc9\n"
        "k1fjVUOq3EIJxGyi51vg9ZZMwbak/17UxFONSjyBAoGAFSQ/HYzZZB3dBMLsiGLs\n"
        "D6JY6bBB7/jczXDHL1U+zZjUyIL9l3DLmjQ1a3eRQJYBx3KRGVdAbXGGjFyuF88M\n"
        "ORfKbBG02E9Jsmh4NgHBBbgVUiDPV0nDKiYUo9Z5poa2jkNSRdCdyN3ucFDTwuXA\n"
        "NgEzfMq7E1nq5cutGomxi+Q=\n"
        "-----END PRIVATE KEY-----\n";

    const char CERTIFICATE[] =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDmDCCAoACCQCqBKAhoAHZ9DANBgkqhkiG9w0BAQsFADCBjTELMAkGA1UEBhMC\n"
        "WEUxGTAXBgNVBAgMEEV4YW1wbGUgUHJvdmluY2UxFTATBgNVBAcMDEV4YW1wbGUg\n"
        "Q2l0eTEVMBMGA1UECgwMRXhhbXBsZSBMdGQuMRQwEgYDVQQDDAtleGFtcGxlLm9y\n"
        "ZzEfMB0GCSqGSIb3DQEJARYQdXNlckBleGFtcGxlLm9yZzAeFw0xNTA4MDkxMDM5\n"
        "MzdaFw00MjEyMjUxMDM5MzdaMIGNMQswCQYDVQQGEwJYRTEZMBcGA1UECAwQRXhh\n"
        "bXBsZSBQcm92aW5jZTEVMBMGA1UEBwwMRXhhbXBsZSBDaXR5MRUwEwYDVQQKDAxF\n"
        "eGFtcGxlIEx0ZC4xFDASBgNVBAMMC2V4YW1wbGUub3JnMR8wHQYJKoZIhvcNAQkB\n"
        "FhB1c2VyQGV4YW1wbGUub3JnMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
        "AQEAoPhrooyLUS+LihegtG5LmIcR671s7tE6XE/dnlafoMxG6rP4FxsYebCUq6OK\n"
        "lYqAwZQbnn+H3b4tU795bOGz8cfzgHUPhWqJpD8utIQo2hD7zp9xLPJvgMS42cTg\n"
        "cm8cyZEkaI4mMsxGXe9HjF8+QFeNT7l0FHiA+sHry3pF2O0lbjlnfVu0qWx5SFVN\n"
        "HKntJ/Aga+cR36f51TfdNIE1SneEnpyLehE5uM4occDDLACiV5BN1UEqNv+Vsigb\n"
        "jDfmswIQZCIVvhxJ8ULOLg5R8SCYRhEip7MwH65F66uMbgwkyWwmV0Ey4Wv46Fbe\n"
        "15Gh/SfvVnjQ/DwPb74KW1aPxQIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQA+sn4F\n"
        "WipuyqmmPRQPSGHUuzBFmKuhQr0zAQl0PEWh33Uu3NxssErcB1H4+ib8FRZbSCQK\n"
        "dGAAM4Rl4HqQ9skYhhI84EUu9Tcey/cnnPqIHPVHDtJEa9kQI4r26Yklwfv2F1DL\n"
        "GvhhR7FWcBc+rETRJQBbhFKGWN3b2le5RfPI0O6veDJgAMSNJy83iOZWOXBi+auq\n"
        "Jz0Dyy+DLoIOZPcoHfyb9Ddp02SA92B1++kh5YteR6CLmFpPYVltUJU8FurkXPhN\n"
        "5WuENasfFTej4qc0jIbC0MrIobVuihPi/aQU7U745audLpH+0hob7YCUw/SfqW9u\n"
        "rwRgtRCPe8HMwKfr\n"
        "-----END CERTIFICATE-----\n";
}

/** Test connect(). */
void
TestNetSecureNetworkStack::testConnect()
{
    // Test connect to nonexistant service. It should fail.
    try {
        afl::net::SecureNetworkStack ns(afl::net::NetworkStack::getInstance());
        bool ok = false;
        try {
            Name name("localhost", "26");     // 26 is unassigned, we assume nothing runs there
            Ref<afl::net::Socket> sock = ns.connect(name, 1000);
            if (&sock.get() != 0) {
                // If it returns, it must be nonzero
                ok = true;
            }
        }
        catch (afl::except::FileProblemException&) {
            // Failed with error, is also ok
            ok = true;
        }
        TS_ASSERT(ok);
    }
    catch (afl::except::UnsupportedException&) {
        // ok
    }
    catch (...) {
        TS_ASSERT(false);
    }
}

/** Test data transfer. */
void
TestNetSecureNetworkStack::testTransfer()
{
    try {
        // Create and configure context
        Ref<SecureContext> serverContext = SecureContext::create();
        serverContext->setCertificate(afl::string::toBytes(CERTIFICATE));
        serverContext->setPrivateKey(afl::string::toBytes(PRIVATE_KEY));

        // Create random name
        Name name("127.0.0.1", uint16_t(std::rand() % 10000 + 20000));

        // Create network stack and listen
        afl::net::SecureNetworkStack sns(afl::net::NetworkStack::getInstance(), serverContext);
        Ref<afl::net::Listener> listener(sns.listen(name, 10));

        // A thread to do the connecting
        class ClientRunnable : public afl::base::Stoppable {
         public:
            ClientRunnable(const Name& name)
                : m_name(name)
                { }
            void run()
                {
                    // Create client network stack
                    afl::net::SecureNetworkStack sns(afl::net::NetworkStack::getInstance());

                    // Connect
                    Ref<Socket> sock(sns.connect(m_name));
                    TS_ASSERT(&sock.get());

                    // Send data
                    static const uint8_t data[] = { 1, 2 };
                    afl::async::SendOperation op(data);
                    afl::async::Controller ctl;
                    bool result = sock->send(ctl, op);
                    TS_ASSERT(result);
                    TS_ASSERT_EQUALS(op.getNumSentBytes(), 2U);
                }
            void stop()
                { }
         private:
            Name m_name;
        };
        ClientRunnable client(name);
        afl::sys::Thread t("ClientThread", client);
        t.start();

        // Do the listening
        Ptr<Socket> sock(listener->accept());
        TS_ASSERT(sock.get());

        // Do the receiving
        uint8_t buf[10];
        afl::async::ReceiveOperation op(buf);
        afl::async::Controller ctl;
        bool result = sock->receive(ctl, op);
        TS_ASSERT(result);
        TS_ASSERT_EQUALS(op.getNumReceivedBytes(), 2U);

        // Tear down
        t.join();
    }
    catch (afl::except::UnsupportedException&) {
        // ok
    }
    catch (...) {
        TS_ASSERT(false);
    }
}
