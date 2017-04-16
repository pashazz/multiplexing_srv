#pragma once
/**
 * @class Multiplexer
 * Abstract a multiplexing method for a server
 */
#include <pthread.h>
#include <functional>
#include <netinet/in.h>

typedef void *(*thread_func)(void*);
class Multiplexer
{
public:

    void setCallback(thread_func callback) { _accept_callback = callback; }
    virtual void startLoop(int server_socket) = 0;

    void acceptConnection(int socket);

protected:
    Multiplexer();
    virtual ~Multiplexer();
    pthread_t  createThread(int client_socket);
    thread_func _accept_callback = nullptr;

private: //for acceptConnection
      struct sockaddr_in client_address;
      socklen_t client_address_size;
};


class SelectMultiplexer : public Multiplexer
{
    void startLoop(int server_socket) override;


};
