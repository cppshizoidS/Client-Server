#include <cstdlib>
#include <iostream>
#include "../server/client_list.hpp"


int main (int argc, char *argv []) {
  if(argc < 2){
    std::cerr << "There is not enough arguments: " << argc << ". Need two.\n";
    return 0;
  }
  
  std::uint8_t start = static_cast<std::uint8_t>(std::atoi(argv[1]));
  std::uint8_t step = static_cast<std::uint8_t>(std::atoi(argv[2]));

  sequence<std::uint8_t> test_seq(start, step);
    
  for (std::size_t i = 0; i < 10 ; ++i) {
    std::cout << +test_seq.get_counter()  << " ";
    ++test_seq;
  }
  return 0;
}
