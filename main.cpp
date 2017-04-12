#include "argparse.hpp"

#include<iostream>

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
    parser.addArgument("--listen", 1, true);
    parser.addArgument("--port", 1, true);
    parser.addArgument("--multiplexer", 1, true);
    parser.parse(argc, argv);



}
