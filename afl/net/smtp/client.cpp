/**
  *  \file afl/net/smtp/client.cpp
  *  \brief Class afl::net::smtp::Client
  */

#include "afl/net/smtp/client.hpp"
#include "afl/io/internalsink.hpp"
#include "afl/net/line/client.hpp"
#include "afl/net/smtp/mailrequest.hpp"

afl::net::smtp::Client::Client(NetworkStack& stack, const Name& name, const Configuration& config)
    : m_networkStack(stack),
      m_name(name),
      m_config(config)
{ }

afl::net::smtp::Client::~Client()
{ }

void
afl::net::smtp::Client::send(afl::base::Memory<const String_t> to, afl::base::ConstBytes_t content)
{
    if (!to.empty()) {
        MailRequest rq(m_config, to, content);
        afl::net::line::Client(m_networkStack, m_name).call(rq);
    }
}

void
afl::net::smtp::Client::send(afl::base::Memory<const String_t> to, const MimeBuilder& content)
{
    afl::io::InternalSink sink;
    content.write(sink, true);
    send(to, sink.getContent());
}
