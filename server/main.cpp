#include <iostream>
#include "server.h"


int main() {
  std::cout << "Server is starting." <<std::endl;
  Settings settings;
  settings.init();
  Server server;
  server.Run();

  return 0;
}
