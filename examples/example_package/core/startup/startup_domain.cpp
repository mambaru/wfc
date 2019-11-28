#include "startup_domain.hpp"

int startup_domain::startup(int argc, char* argv[], std::string helpstring) 
{
  std::cout << "example_startup::startup: " << helpstring << std::endl;
  if ( argc > 1 )
  {
    if ( std::string(argv[1]) == std::string("run"))
      _ready = true;
    else
      return 1;
  }
  return 0;
}
  
bool startup_domain::ready_for_run()
{
  return _ready;
}

void startup_domain::clean_finalize() 
{
  
}
