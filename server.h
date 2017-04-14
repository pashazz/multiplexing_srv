#pragma once

#include "multiplexer.h"
#include <unistd.h>



class Server
{
public:
    /**
       * @brief The thread_data struct
       * Passed to _accept_callback (as void*, but should be converted)
       * Created in dynamic memory so that one should delete it
       */
      struct thread_data
      {
          int socket_fd;
      };

    Server(Multiplexer *mplexer) : _mplexer(mplexer) {}

    void start(std::string host, unsigned short port);

    void setAcceptCallback(thread_func callback) { _acceptCallback = callback; }
protected:
    thread_func _acceptCallback = nullptr;

    Multiplexer *_mplexer = nullptr;

};
