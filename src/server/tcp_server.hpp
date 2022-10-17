#ifndef _TCP_SERVER_HPP_
#define _TCP_SERVER_HPP_

#include "client_list.hpp"

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <sstream>
#include <string>
#include <thread>

/**
 * @brief simple tcp server
 *
 */
class TCP_server
{
public:
  TCP_server() = delete;
  TCP_server(TCP_server &oth) = delete;
  TCP_server(const TCP_server &oth) = delete;
  TCP_server(TCP_server &&oth) = delete;
  TCP_server operator=(const TCP_server oth) = delete;
  TCP_server operator=(TCP_server &&oth) = delete;

  TCP_server(const uint16_t _port,
             std::function<void(std::unique_ptr<int> &&)> _callback)
      : port(_port),
        callback(_callback){};

  void run()
  {
    listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0)
    {
      perror("socket");
      exit(1);
    }
    // reset to zero
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    // we bind to all interfaces
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bool_value = 1;
    setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &bool_value, sizeof(bool_value));
    // clang-format off
    if (bind(listener_socket, (struct sockaddr*)&servaddr, sizeof(sockaddr)) < 0) {
      printf("error bind ");
      exit(1);
    }
    // clang-format on
    if (listen(listener_socket, 0))
    {
      printf("error lisner");
      exit(1);
    }

    while (not terminate)
    {
      std::unique_ptr<int> socket_client(new int);
      *socket_client = (accept(listener_socket, NULL, NULL));
      // We have to process new client therefore move it to new thread and process in it
      std::thread(callback, std::move(socket_client)).detach();
    }

    close(listener_socket);
  }

private:
  int listener_socket;
  struct sockaddr_in servaddr;
  struct sockaddr_in cliet_addr;
  uint16_t port;
  std::function<void(std::unique_ptr<int> &&)> callback;
  int *socket_client{nullptr};

public:
  static bool terminate;
};

bool TCP_server::terminate = false;

#endif //_TCP_SERVER_HPP_
