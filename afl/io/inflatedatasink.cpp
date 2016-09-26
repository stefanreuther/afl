/**
  *  \file afl/io/inflatedatasink.cpp
  *  \brief Class afl::io::InflateDataSink
  */

#include <cassert>
#include "afl/io/inflatedatasink.hpp"

const afl::io::InflateDataSink::Personality afl::io::InflateDataSink::Gzip;
const afl::io::InflateDataSink::Personality afl::io::InflateDataSink::Raw;


afl::io::InflateDataSink::InflateDataSink(DataSink& peer, Personality personality)
    : TransformDataSink(peer)
{
    setNewTransform(new InflateTransform(personality));
}

afl::io::InflateDataSink::~InflateDataSink()
{ }

bool
afl::io::InflateDataSink::isAvailable()
{
    return InflateTransform::isAvailable();
}
