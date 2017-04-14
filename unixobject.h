#pragma once
#include <errno.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <iostream>

/// Генерация форматированной строки вывода
template <typename... Ts>
std::string string_format(const std::string &fmt, Ts... vs)
{
  char b;
  unsigned required = std::snprintf(&b, 0, fmt.c_str(), vs...) + 1;
  // the +1 is necessary, while the first parameter
  //               can also be set to nullptr

  //    char bytes[required];
  std::vector<char> bytes(required);
  std::snprintf(bytes.data(), required, fmt.c_str(), vs...);

  return std::string(bytes.data());
}

class SystemCallException
{
public:
    SystemCallException(const std::string &file, int line, const std::string &func, const std::string &call)
        :file(file), line(line), func(func), call(call) {}
    std::string file;
    int line;
    std::string func;
    std::string call;


    void print()
    {
        perror(string_format("%s:%d (in %s): system call error (%s)", file.c_str(), line, func.c_str(), call.c_str()).c_str());
    }
};

#define SYSC_ERR(call) throw SystemCallException(__FILE__, __LINE__, __func__, call)

/**
 * @brief The SocketReader class
 * Read CLRF lines from socket using recv
 */
class HTTPSocketReader
{
public:
    HTTPSocketReader(int socket) : socket(socket) {}

    /**
     * @brief readLine
     * @return a line from socket w/o \r\n
     */
    std::string readLine()
    {
        if (!str.empty())
        {
            auto pos = str.find("\r\n");
            if (pos == std::string::npos && !str.empty())
            {
                return str; //either an empty or non-full line
            }
            else
            {
                std::string ret = str.substr(0, pos);
                if (str.length() > pos+2) //\r\n
                    str = str.substr(pos+2);
                else
                    str.clear();
                return ret;
            }
        }
        else
        {
            std::vector<char> buffer;
            buffer.resize(BUFSIZ);
            auto newSize = recv(socket, buffer.data(), buffer.size(), 0);
            if (newSize == 0)
                return std::string();
            if (newSize == -1)
                SYSC_ERR("recv");

            buffer.resize(newSize);
            str += std::string(buffer.data());
            return readLine();
        }

     }

    /**
     * @brief readContent
     * @return message body as read using Content-Length attribute
     */
    std::string readContent()
    {
        size_t length = 0;
        while(true)
        {
            std::string str = readLine();
            auto pos = str.find(":");
            if (pos != std::string::npos)
            {
                std::string header = str.substr(0, pos);
                if (header == "Content-Length" && str.length() > pos+1)
                {
                    //convert length to long

                    std::string value=str.substr(pos+1);
                    length = strtol(value.c_str(), nullptr, 10);
                }
            }
            std::cerr << str << std::endl;
            if (str.empty())
                break;
        }
        if (length == 0)
        {
            return readToEnd();
        }
        if (str.size() < length)
        {
            std::vector<char> buffer(length - str.size());
            if (recv(socket, buffer.data(), buffer.size(), 0) == -1)
                SYSC_ERR("recv");
            str += std::string(buffer.data());
        }
        std::string ret = str;
        str.clear();
        return ret;

    }

    std::string readToEnd()
    {
        while(true)
        {
            std::vector<char> buffer;
            buffer.resize(BUFSIZ);
            auto newSize = recv(socket, buffer.data(), buffer.size(), 0);
            if (newSize == 0)
                break;
            if (newSize == -1)
                SYSC_ERR("recv");

            buffer.resize(newSize);
            str += std::string(buffer.data());
        }
        auto ret = str;
        str.clear();
        return ret;
    }

protected:
    std::string str;
    int socket;

};
