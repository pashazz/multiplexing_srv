#include "multiplexer.h"
#include <unistd.h>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "unixobject.h"
#include <pthread.h>
#include <netinet/in.h>
using namespace std;

void SelectMultiplexer::startLoop(int server_socket)
{
    if (!_accept_callback)
        throw runtime_error("no callback given for multiplexer");

    int fd_size = getdtablesize();
    fd_set following; //for sending to select()
    fd_set active; // our set comprising server socket

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(struct sockaddr_in));
    socklen_t client_address_size = sizeof(client_address);
    FD_ZERO(&active);
    FD_SET(server_socket, &active);



    while(true)
    {
        memcpy(&following, &active, sizeof(fd_set));
        if (select(fd_size, &following, nullptr, nullptr, nullptr) == -1)
            SYSC_ERR("select");

        if (FD_ISSET(server_socket, &following))
        {
            int client_socket = accept(server_socket,
                                       reinterpret_cast<struct sockaddr*>(&client_address),
                                       &client_address_size);
            if (client_socket == -1)
                SYSC_ERR("accept");

            //create a new thread (TODO)
        }
    }
}

