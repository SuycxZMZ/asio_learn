#include <iostream>
#include "endpoints.h"

int main() {

  std::cout << "-------------- test boostasio -------------\n";
  std::cout << "client_end_point : " << client_end_point() << std::endl;
  std::cout << "server_end_point : " << server_end_point() << std::endl;

  return 0;
}