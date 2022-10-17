#include "server/client_handler.hpp"
#include "server/tcp_server.hpp"

#include <signal.h>
#include <functional>
#include <sstream>
#include <string>

/**
 * @brief Handle signal from OS
 *
 * @param signum
 * @details close all sockets and file descriptors
 */
void signal_handler(int)
{
  TCP_server::terminate = true;
  client_handler::terminate = true;
  client_list::get_instance()->close_all_socket();

  exit(1);
}

int main(int argc, char *argv[])
{
  // Handle signals from OS
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGTERM, signal_handler);

  // ignore SIG , when we send close socket ;
  signal(SIGPIPE, SIG_IGN);


  if (argc < 2)
  {
    std::cout << "plesse set port " << std::endl;
    return 0;
  }

  uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));

  client_handler cl_handler;

  auto handler = [&cl_handler](std::unique_ptr<int> &&socket_client) -> void
  {
    cl_handler.service(std::move(socket_client));
  };

  TCP_server server(port, handler);
  server.run();
}
