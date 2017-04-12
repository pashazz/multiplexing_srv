#include "server.h"
#include <sys/socket.h> //socket
#include <sys/types.h>
#include <string>
#include <cstring>
#include <arpa/inet.h> //in_addr

#include "unixobject.h"
using namespace std;

void Server::start(string host, unsigned short port)
{
    //Create a socket (TCP)
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        SYSC_ERR("socket");

    //convert address
    struct in_addr addr;
    memset(&addr, 0, sizeof(struct in_addr));


    if (inet_aton(host.c_str(), &addr) != 1)
        throw runtime_error(string_format("Malformed address: %s", host.c_str()));

    //create socket address
    struct sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_in));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = addr.s_addr;
    sock_addr.sin_port = htons(port);

    //bind
    if (bind(_socket, reinterpret_cast<struct sockaddr*>(&sock_addr),
             sizeof(struct sockaddr)) == -1)
        SYSC_ERR("bind");

    //listen
    if (listen(_socket, SOMAXCONN) == -1)
        SYSC_ERR("listen");




}
