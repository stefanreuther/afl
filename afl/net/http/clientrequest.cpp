/**
  *  \file afl/net/http/clientrequest.cpp
  *  \brief Interface afl::net::http::ClientRequest
  */

#include "afl/net/http/clientrequest.hpp"

String_t
afl::net::http::toString(ClientRequest::FailureReason r)
{
    switch (r) {
     case ClientRequest::Cancelled:           return "Cancelled";
     case ClientRequest::ConnectionFailed:    return "ConnectionFailed";
     case ClientRequest::ConnectionClosed:    return "ConnectionClosed";
     case ClientRequest::UnsupportedProtocol: return "UnsupportedProtocol";
     case ClientRequest::NetworkError:        return "NetworkError";
     case ClientRequest::ServerError:         return "ServerError";
    }
    return "?";
}
