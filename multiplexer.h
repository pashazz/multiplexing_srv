#pragma once
/**
 * @class Multiplexer
 * Abstract a multiplexing method for a server
 */
#include <pthread.h>
#include <functional>
typedef void *(*thread_func)(void*);
class Multiplexer
{
public:

    void setCallback(thread_func callback) { _accept_callback = callback; }
    virtual void startLoop(int server_socket) = 0;

protected:
    pthread_t  createThread(int client_socket);
    thread_func _accept_callback = nullptr;

};


class SelectMultiplexer : public Multiplexer
{
    void startLoop(int server_socket) override;

};
