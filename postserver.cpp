#include "postserver.h"
#include <unistd.h>
#include "unixobject.h"
#include <iostream>
#include <openssl/sha.h>
#include <array>
#include <json/json.h>
#include <cstring>
#include <json/writer.h>
/**

Данный веб-сервер должен реализовывать один HTTP-route "/hash", доступный при использовании HTTP-метода POST; все остальные запросы и методы должны возвращать HTTP статус-код 409 с пустым телом ответа.
В качестве тела принимаемого сообщения должен восприниматься JSON вида {"str": "random string", "rounds": unsigned int}; в качестве ответа должен выдаваться корректный HTTP-ответ, соответствующий стандарту HTTP 1.0 с JSON вида
{"str": "input string", "rounds": unsigned int, "sha512":  "hash"},
 где под input sting подразумевается принятая на вход строка, под unsigned int — переданное на вход целое беззнаковое число,
 а под hash подразумевается построенный над этой строкой SHA512 хэш с использованием N раундов.
Соль использовать не нужно.
Для построения хэша вам необходимо использовать одну и ту же библиотеку; в случае отсутствия возражений я предлагаю использовать стандартную библиотеку crypt : https://linux.die.net/man/3/crypt
 */

using namespace std;

std::string sha512 (std::string input)
{
    std::array<unsigned char, SHA512_DIGEST_LENGTH> digest;
    memset(digest.data(), 0, digest.size());
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, input.c_str(), input.length());
    SHA512_Final(digest.data(), &ctx);

    char mdString[SHA512_DIGEST_LENGTH*2+1];
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
    return std::string (mdString);
}



extern "C" void * accept_callback (void *data)
{
    auto thread_data = static_cast<Server::thread_data*>(data);
    //Read HTTP request
    int socket = thread_data->socket_fd;
    delete thread_data;
    HTTPSocketReader reader(socket);

    string line;
    try
    {
        line = reader.readLine(); //LINE 1. Should be POST /hash HTTP/1.0
        if (line != "POST /hash HTTP/1.0")
        {
            //Send 409 Conflict
            const char * err_response = "HTTP/1.0 409 Conflict\r\n\r\n";
            if (send(socket, err_response, strlen(err_response), 0) == -1)
                SYSC_ERR("send");
        }
        else
        {
            //Read the message
            auto message = reader.readContent();
            Json::Value root;
            Json::Reader jreader;
            if (!jreader.parse(message, root))
            {
                std::cerr << "Failed to parse JSON request msg body:\n " << jreader.getFormattedErrorMessages() << std::endl;
                const char * err_response = "HTTP/1.0 400 Bad Request\r\n\r\n";
                if (send(socket, err_response, strlen(err_response), 0) == -1)
                    SYSC_ERR("send");
                close(socket);
                return nullptr;
            }
            auto str = root.get("str", "").asString();
            auto rounds = root.get("rounds", 0).asUInt();
            if (str.empty())
            {
                std::cerr << "Wrong attribute value of JSON object" << std::endl;
                const char * err_response = "HTTP/1.0 400 Bad Request\r\n\r\n";
                if (send(socket, err_response, strlen(err_response), 0) == -1)
                    SYSC_ERR("send");
            }
            else
            {
                auto hash = sha512(str);
                Json::Value response;
                response["str"]=str;
                response["rounds"]=rounds;
                response["hash"]=hash;
                Json::FastWriter writer;
                auto msgBody = writer.write(response);
                const char * answerTemplate="HTTP/1.0 200 OK\r\n"
                                    "Content-Length: %d\r\n"
                                    "Content-Type: application/json\r\n\r\n"
                                    "%s";
                auto  responseString = string_format(answerTemplate, msgBody.length(), msgBody.c_str());
                if (send(socket, responseString.c_str(), responseString.size(), 0) == -1)
                    SYSC_ERR("send");
            }
        }

    }
    catch (SystemCallException ex)
    {
        ex.print();
        return nullptr;
    }


    close(socket);
    return nullptr;
}
