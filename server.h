#pragma once

#include "multiplexer.h"

class Server
{
public:
    Server(Multiplexer *mplexer) : _mplexer(mplexer) {}

    void start(std::string host, unsigned short port);

private:
    int _socket;
    Multiplexer *_mplexer = nullptr;

};
