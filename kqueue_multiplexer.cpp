#include "kqueue_multiplexer.h"
#ifdef BSD
#include "unixobject.h"
#include <sys/event.h>
#include <netinet/in.h>
using namespace std;
void KqueueMultiplexer::startLoop(int server_socket)
{
    if (!_accept_callback)
        throw runtime_error("no callback given for Kqueue multiplexer");

    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(sockaddr_in));
    socklen_t client_address_size = sizeof(client_address);


    struct kevent monitored;
    struct kevent event;
    int queue = kqueue();
    if (queue == -1)
        SYSC_ERR("kqueue");

    EV_SET(&monitored, server_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(queue, &monitored, 1, NULL, 0, NULL) == -1)
        SYSC_ERR("kevent");

    if (monitored.flags & EV_ERROR)
        throw runtime_error(string_format("Failed to register kevent event: error: %s", strerror(monitored.data)));

    //Monitor
    while(true)
    {
        int nevents = kevent(queue, NULL, 0, &event, 1, NULL);
        if (nevents == -1)
            SYSC_ERR("kevent");

        if (nevents > 0)
        {
            if (event.ident == server_socket)
            {
                for (int i = 0; i < event.data; ++i)
                { //event.data contains backlog size
                    acceptConnection(event.ident);
                }
            }
            else
            {
                std::cerr << "Kqueue error: pending descriptor: " << event.ident << "; socket descriptor: " << server_socket << std::endl;
            }
        }
    }

}
#endif
