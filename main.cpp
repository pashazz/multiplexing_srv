#include "argparse.hpp"
#include <pthread.h>
#include<iostream>
#include "postserver.h"
#include "unixobject.h"

/**
 * @brief main Main method
 * @param argc
 * @param argv
 * Command arguments
 * -listen <ip> an IP address to listen(2) to
 * -port <no> port number
 * -multiplexer <multiplexer type>. This program only supports kqueue and select
 * @return
 */
int main(int argc, const char** argv)
{
    ArgumentParser parser;
    parser.addArgument("--listen", 1, false);
    parser.addArgument("--port", 1, false);
    parser.addArgument("--multiplexer", 1, false);
    parser.parse(argc, argv);

    auto host = parser.retrieve<std::string>("listen");
    auto port_str = parser.retrieve<std::string>("port");
    long port = strtol(port_str.c_str(), nullptr, 10);
    //Multiplexer is now select

    auto mplexer = new SelectMultiplexer;
    auto server = new Server(mplexer);
    server->setAcceptCallback(accept_callback); //from postserver.h
    try
    {
        server->start(host, port);
    }
    catch (SystemCallException ex)
    {
        ex.print();
    }

    pthread_exit(nullptr);
}
