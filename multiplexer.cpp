#include "multiplexer.h"
#include <unistd.h>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "unixobject.h"
#include <pthread.h>
#include <netinet/in.h>
#include "server.h"
using namespace std;


void SelectMultiplexer::startLoop(int server_socket)
{
    if (!_accept_callback)
        throw runtime_error("no callback given for multiplexer");

    fd_set following; //for sending to select()
    fd_set active; // our set comprising server socket

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(sockaddr_in));
    socklen_t client_address_size = sizeof(client_address);
    FD_ZERO(&active);
    FD_SET(server_socket, &active);



    while(true)
    {
        memcpy(&following, &active, sizeof(active));
        if (select(FD_SETSIZE, &following, (fd_set*)0, (fd_set*)0, (struct timeval*)0) == -1)
            SYSC_ERR("select");

        if (FD_ISSET(server_socket, &following))
        {
            int client_socket = accept(server_socket,
                                       reinterpret_cast<struct sockaddr*>(&client_address),
                                       &client_address_size);
            if (client_socket == -1)
                SYSC_ERR("accept");

            //create a new thread (TODO)

            createThread(client_socket);
        }
    }
}


pthread_t Multiplexer::createThread(int client_socket)
{
    pthread_t tid;
    auto data = new Server::thread_data;
    data->socket_fd = client_socket;
    pthread_create(&tid, nullptr, _accept_callback, data);
    return tid;
}
