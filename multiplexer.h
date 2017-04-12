#pragma once
/**
 * @class Multiplexer
 * Abstract a multiplexing method for a server
 */

#include <functional>
class Multiplexer
{
public:
    void setCallback(std::function<void(int)> callback) { _accept_callback = callback; }
    virtual void startLoop(int server_socket) = 0;
protected:
    std::function<void(int)> _accept_callback = nullptr;
};


class SelectMultiplexer : public Multiplexer
{
    void startLoop(int server_socket) override;
};
