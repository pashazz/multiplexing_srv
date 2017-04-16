#pragma once
#include "multiplexer.h"
#include <sys/param.h>
#ifdef BSD
class KqueueMultiplexer : public Multiplexer
{
  void startLoop(int server_socket) override;
};

#endif
