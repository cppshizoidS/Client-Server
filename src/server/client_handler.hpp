#ifndef _CLIENT_HANDLER_HPP_
#define _CLIENT_HANDLER_HPP_

#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <memory>
#include <regex>

#include <iostream>

#include "client_list.hpp"
#include <thread>


class client_handler
{
public:
  void service(std::unique_ptr<int> &&socket_client)
  {
    client_settings client;
    client.seq.resize(3);
    client.socket = (std::move(socket_client));
    client_list *cl_list = client_list::get_instance();

    if (client_dialog(client))
    {
      std::uint64_t id_client = client_list::get_instance()->emplace_back(std::move(client));
      std::cout << "ADD client, active client is " << cl_list->active_clients() << std::endl;
      send_data(cl_list->at(id_client));

      cl_list->erase(id_client);
      std::cout << "DELETE client, active client is " << cl_list->active_clients() << std::endl;
      return;
    }
    std::cout << "IGNORE client, active client is " << cl_list->active_clients() << std::endl;

    return;


private:

  bool client_dialog(client_settings &new_client)
  {
    ssize_t msg_size{0};
    new_client.seq.resize(3);

    while (not terminate)
    {
      bzero(msg_buf, len);
      msg_size = recv(*new_client.socket, (void *)msg_buf, len, 0);
      if (msg_size < 0)
      {
        // handle error client;
        perror("error client ");
        return false;
      }

      std::string msg_str(msg_buf, msg_size);

      std::sregex_iterator iter(msg_str.begin(), msg_str.end(), regexp_seq);
      std::sregex_iterator end;
      if (iter->size() > 0)
      {
        try
        {
          std::uint64_t seq_iter = std::stoull((*iter)[1]);
          std::uint64_t start = std::stoull((*iter)[2]);
          std::uint64_t step = std::stoull((*iter)[3]);

          new_client.seq.at(seq_iter - 1).set(start, step);

#ifdef DEBUG_PRINT
          std::cout << "seq_iter " << seq_iter << " ; start " << start << "; step " << step << std::endl;
#endif // DEBUG_PRINT
        }
        catch (const std::out_of_range &e)
        {
          std::string error{"Is too long for unsigned long long (uint64_t)\n"};
          if (send(*new_client.socket, error.c_str(), error.size() + 1, 0) < 0)
          {
            perror("connection error");
            return false;
          }
          continue;
        }

        continue;
      }

      if (msg_str.find(command_start_job) != std::string::npos)
      {

#ifdef DEBUG_PRINT
        std::cout << "client seq is " << new_client.seq.size() << std::endl;
        for (auto &it : new_client.seq)
        {
          std::cout << it.get_counter() << " ";
        }
        std::cout << std::endl;
#endif
        // remove invalid sequence
        auto it = std::remove_if(new_client.seq.begin(), new_client.seq.end(), [](sequence<uint64_t> &seq_)
                                 { return not seq_.is_valid(); });
        new_client.seq.erase(it, new_client.seq.end());

        if (not new_client.seq.empty())
        {
          return true;
        }
        else
        {
          new_client.seq.resize(3);
        }
      }

      if (send(*new_client.socket, unrecognized_command_msg.c_str(), unrecognized_command_msg.size() + 1, 0) < 0)
      {
        perror("error client ");
        return false;
      }
    }
    return false;
  }

  void send_data(client_settings &client)
  {
    std::string message;
    message.reserve(1522);

    while (not terminate)
    {
      message.clear();
      for (int i = 0; i < 1024; ++i)
      {
        for (auto &seq : client.seq)
        {
          message += std::to_string(seq.get_counter()) + " ";
          ++seq;
        }
        message += '\n';
      }

      //check socket connection, if it closes then delete client or set flag or them

      if (send(*client.socket, message.c_str(), message.size(), 0) < 0)
      {
        std::cout << "connection lost" << std::endl;
        return;
      }
    }
  }

private:
  static constexpr size_t len{256};
  char msg_buf[len];

  std::string command_start_job = "export seq";
  std::string unrecognized_command_msg = "the wrong command could not be recognized, please repeat the input \n";
  std::regex regexp_seq{"^seq([1-3]) ([0-9]+) ([0-9]+)"};

public:
  static bool terminate;
};

bool client_handler::terminate = false;

#endif //_CLIENT_HANDLER_HPP_
