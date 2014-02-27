#include <wfc/io/ip/tcp/rn/server.hpp>
#include <wfc/io_service.hpp>

#include <thread>
#include <algorithm>

void test(wfc::io_service& io_service)
{
  io_service.stop();
}

typedef wfc::io::ip::tcp::rn::server server;

void handler(server::data_ptr d, server::handler_callback callback)
{
  std::reverse( d->begin(), d->end() );
  callback( std::move(d) );
}

int main(int argc, char* /*argv*/[])
{
  
  server::config conf;
  conf.threads = 10;
  wfc::io_service io_service;
  wfc::io_service::work wrk(io_service);
  
  server srv(io_service);
  srv.configure(conf);
  srv.initialize(handler);
  srv.start();
  
  std::thread th;
  
  if (argc == 1)
    th = std::thread( std::bind(test, std::ref(io_service) ) );
  
  std::cout << "run" << std::endl;
  io_service.run();
  std::cout << "done" << std::endl;
  
  srv.stop();
  
  if (argc == 1)
    th.join();
}
