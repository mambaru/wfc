#include <wfc/inet/srv/ip/tcp/aspect.hpp>
#include <wfc/inet/srv/ip/tcp/config.hpp>
#include <wfc/inet/server.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
  if ( argc < 2 )
  {
    std::cerr << "Usage: inet1-srv <port> " << std::endl;
  }
 
 boost::asio::io_service io;
 wfc::inet::srv::ip::tcp::config conf;
 
 typedef wfc::inet::server<
   wfc::inet::server_aspect< wfc::inet::srv::ip::tcp::aspect>
 > server;
 
 conf.host = "0.0.0.0";
 conf.port = argv[1];
 server srv( io, conf);
  
}