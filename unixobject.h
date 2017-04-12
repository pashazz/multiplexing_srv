#pragma once
#include <errno.h>
#include <vector>
#include <string>
#include <stdio.h>

/// Генерация форматированной строки вывода
template <typename... Ts>
const char* string_format(const std::string &fmt, Ts... vs)
{
  char b;
  unsigned required = std::snprintf(&b, 0, fmt.c_str(), vs...) + 1;
  // the +1 is necessary, while the first parameter
  //               can also be set to nullptr

  //    char bytes[required];
  std::vector<char> bytes(required);
  std::snprintf(bytes.data(), required, fmt.c_str(), vs...);

  return bytes.data();
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
        perror(string_format("%s:%d (in %s): system call error (%s): ", file.c_str(), line, func.c_str(), call.c_str()));
    }
};

#define SYSC_ERR(call) throw SystemCallException(__FILE__, __LINE__, __func__, call)
