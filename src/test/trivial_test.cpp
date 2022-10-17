#include <signal.h>
#include <functional>
#include <sstream>
#include <string>

#include "../server/client_handler.hpp"
#include "../server/tcp_server.hpp"
#include "../thread_pool/thread_pool.hpp"

void signal_handler(int signum) {
  TCP_server::terminate = true;
  client_handler::terminate = true;
  client_list::get_instance()->close_all_socket();

  /* Close all open file descriptors */
  int x;
  for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
    close(x);
  }
  exit(1);
}


void print_chunk_data (client_settings& client) {
    std::string message;
    message.reserve(1522);
    for (int i = 0; i < 100; ++i) {
      for (auto& seq : client.seq) {
        message += std::to_string(seq.get_counter()) + " ";
        ++seq;
      }
      message += '\n';
    }
    send(*client.socket, message.c_str(), message.size(), NULL );
  
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
    client.in_process = false;
    return;
}

int main(int argc, char* argv[]) {
  
  signal(SIGINT, signal_handler);
  signal(SIGTSTP, signal_handler);
  signal(SIGTERM, signal_handler);

  if (argc < 2 ) {
    std::cout << "plesse set port " << std::endl;
    return 0;
  }

  uint16_t port  = std::atoi(argv[1]);

  auto server_run = [](uint16_t port) {
    client_handler handler;
    auto callback =
        std::bind(&client_handler::service, &handler, std::placeholders::_1);
    TCP_server server(port, callback);
    server.run();
  };

  auto task = []( client_settings& client)  {
    //if (client.in_processing) {return;}
    // ethernet patket data size
    std::string message;
    message.reserve(1522);
    for (int i = 0; i < 3 ; ++i) {
      for (auto& seq : client.seq) {
        message += std::to_string(seq.get_counter()) + " ";
        ++seq;
      }
      message += '\n';
    }
    send(*client.socket, message.c_str(), message.size(), NULL );
    using namespace std::chrono_literals;
   // client.in_processing = false;
    #ifdef DEBUG
    std::this_thread::sleep_for(500ms);
    #endif // DEBUG
    return;
  };

  std::thread(server_run, port).detach();
  thread_pool thr_pool(2);
  
  client_list* cli_list =client_list::get_instance();
  while ( not TCP_server::terminate )   {
    for (auto & client:  *cli_list) {
     if ( not client.in_process) {   
      client.in_process = true;
      thr_pool.add_task(print_chunk_data, std::ref(client));
     }
    }
    std::this_thread::yield();
  }


}