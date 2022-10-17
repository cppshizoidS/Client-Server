#ifndef _CLIENT_LIST_HPP_
#define _CLIENT_LIST_HPP_

#include <unistd.h>

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>


template <typename T>
class sequence
{
public:
  sequence() = default;
  sequence(sequence &&oth) = default;
  sequence &operator=(sequence &&oth) = default;

  sequence(sequence &oth) = delete;
  sequence(const sequence &oth) = delete;
  sequence &operator=(const sequence &oth) = delete;


  sequence(T start_, T step_)
      : start(start_),
        step(step_),
        counter(start_){

        };

  void set(T start_, T step_)
  {
    start = start_;
    step = step_;
    counter = start_;
  }

  bool is_valid()
  {
    return (start > 0ul) && (step > 0ul);
  }

  sequence &operator++()
  {
    auto tmp_counter = counter;
    if (std::numeric_limits<T>::max() - step < tmp_counter)
    {
      // Edge case: there is counter overflow, have to return to begin value
      counter = start;
    }
    else
    {
      counter = static_cast<T> (counter + step);
    }
    return *this;
  }

  T get_counter() { return counter; }

  std::string get_param_str() const
  {
    return (std::string(" strt > ") + std::to_string(start) + "; step" + std::to_string(step) + "; counter " + std::to_string(counter));
  }

private:
  T start;
  T step;
  T counter;
};


struct client_settings
{
  client_settings()
  {
    client_id = counter;
    ++counter;
  }

  client_settings(client_settings &&oth) = default;

  client_settings(client_settings &oth) = delete;
  client_settings &operator=(const client_settings &oth) = delete;

  std::uint64_t client_id;
  std::unique_ptr<int> socket{nullptr};
  std::vector<sequence<std::uint64_t>> seq;

  friend bool operator<(const client_settings &lft, const client_settings &rht)
  {
    return lft.client_id < rht.client_id;
  }

private:
  static std::uint64_t counter;
};
std::uint64_t client_settings::counter{0};


class client_list
{
public:
  client_list(client_list &oth) = delete;
  client_list(const client_list &oth) = delete;
  client_list(client_list &&oth) = delete;
  client_list &operator=(const client_list &oth) = delete;
  client_list &operator=(client_list &&oth) = delete;

  static client_list *get_instance()
  {
    std::lock_guard<std::mutex> lock(get_client_list_mutex());
    if (not instance_client_list)
    {
      instance_client_list = new client_list;
    }
    return instance_client_list;
  }

  std::uint64_t emplace_back(client_settings &&client)
  {
    std::lock_guard<std::mutex> guard(internal_mutex);
    std::uint64_t id = client.client_id;
    client_l.emplace(client.client_id, std::move(client));
    return id;
  }

  void erase(std::uint64_t it)
  {
    std::lock_guard<std::mutex> guard(internal_mutex);
    client_l.erase(it);
  }

  client_settings &at(uint64_t id)
  {
    return client_l.at(id);
  }

  std::size_t size()
  {
    return client_l.size();
  }

  auto begin()
  {
    return client_l.begin();
  }

  auto end()
  {
    return client_l.end();
  }

  void close_all_socket()
  {
    for (auto & [key, settings] : client_l)
    {
      close(*(settings.socket));
    }
    return;
  }

  auto active_clients() const
  {
    return client_l.size();
  }

 

private:
  client_list() = default;

  static std::mutex &get_client_list_mutex()
  {
    static std::mutex mutex;
    return mutex;
  }

private:
  static client_list *instance_client_list;
  std::mutex internal_mutex;

  std::unordered_map<std::uint64_t, client_settings> client_l;

  uint64_t last_client_id{0};
};

client_list *client_list::instance_client_list{nullptr};

#endif // _CLIENT_LIST_HPP_
