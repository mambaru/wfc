#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <algorithm>

#include <wfc/io/ip/tcp/rn/client.hpp>


#define COUNTER_MAX 1000L
#define PERC0   0
#define PERC20  (COUNTER_MAX*20/100)
#define PERC50  (COUNTER_MAX*50/100)
#define PERC80  (COUNTER_MAX*80/100)
#define PERC95  (COUNTER_MAX*95/100)
#define PERC99  (COUNTER_MAX*99/100)
#define PERC100 (COUNTER_MAX-1)
std::mutex mutex;
std::vector<size_t> counters;

auto gstart = std::chrono::high_resolution_clock::now();
auto gfinish = std::chrono::high_resolution_clock::now();

void stat(size_t ts)
{
  std::lock_guard<std::mutex> lk(mutex);
  counters.push_back( ts );
  
  if ( counters.size() == COUNTER_MAX )
  {
    gfinish = std::chrono::high_resolution_clock::now();
    std::sort(counters.begin(), counters.end());
    std::cout << counters[PERC0] << '\t'
              << counters[PERC20] << '\t'
              << counters[PERC50] << '\t'
              << counters[PERC80] << '\t'
              << counters[PERC95] << '\t'
              << counters[PERC99] << '\t'
              << counters[PERC100] << '\t' 
              << "rate: " << (COUNTER_MAX * 1000000L) / std::chrono::duration_cast < std::chrono::microseconds>( gfinish - gstart ).count()
              << std::endl;
    counters.clear();
    gstart = std::chrono::high_resolution_clock::now();
  }
}

int run_client(int , char *argv[])
{
  
  std::string str = std::string(argv[3]) + std::string("\r\n");
  std::cout << "run_client: " << str << std::endl;
  /*int sockfd, portno, n;*/
  //struct sockaddr_in serv_addr;
  //struct hostent *server;

  //char buffer[256];

  //portno = atoi(argv[2]);
  
  typedef wfc::io::ip::tcp::rn::client client_type;
  typedef client_type::options_type options_type;
  
  wfc::asio::io_service io;
  wfc::asio::io_service::work wrk(io);
  
  options_type options;
  
  options.connection.keep_alive = true;
  client_type *cliptr;
  auto start = std::chrono::high_resolution_clock::now();
  auto finish = std::chrono::high_resolution_clock::now();

  
  options.connection.startup_handler = [&]( wfc::io::io_id_t /*id*/, wfc::io::outgoing_handler_t callback, wfc::io::add_shutdown_handler_t add)
  {
    auto d = std::make_unique< wfc::io::data_type>( str.begin(), str.end() );
    callback( std::move(d) );
    add([&](wfc::io::io_id_t /*id*/){
      start = std::chrono::high_resolution_clock::now();
      cliptr->start();
    });
  };
  
  options.incoming_handler = wfc::io::simple_handler([&]( wfc::io::data_ptr /*d*/, wfc::io::outgoing_handler_t /*callback*/){
    finish = std::chrono::high_resolution_clock::now();
    stat( std::chrono::duration_cast < std::chrono::microseconds>( finish - start ).count() );
  });
  client_type client(io, options);
  cliptr = &client;
  
  start = std::chrono::high_resolution_clock::now();
  client.start();
  io.run();
  
  
  /*
  while (1)
  {
    std::unique_lock<std::mutex> lk(mutex);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
      perror("ERROR opening socket");
      exit(1);
    }
    
    
    {
      int val = 1;
      if ( setsockopt((int)sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&val, sizeof(val)))
      {
        perror("ERROR SO_REUSEPORT");
          exit(1);
      }

      val = 1;
      if ( setsockopt((int)sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&val, sizeof(val)))
      {
        perror("ERROR SO_REUSEADDR");
          exit(1);
      }
      
    }

    
    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    memcpy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
                  server->h_length);
    
    serv_addr.sin_port = htons(portno);
    lk.unlock();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    
    
    if (connect(sockfd, (sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) 
    {
      perror("ERROR connecting");
      exit(1);
    } 
    
    {
      int val = 1;
      if ( setsockopt((int)sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&val, sizeof(val)))
      {
        perror("ERROR SO_REUSEPORT");
          exit(1);
      }

      val = 1;
      if ( setsockopt((int)sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&val, sizeof(val)))
      {
        perror("ERROR SO_REUSEADDR");
          exit(1);
      }
    }

    
    
    
    n = write(sockfd, str.c_str(), str.size() );
    if (n < 0) 
    {
      perror("ERROR writing to socket");
      exit(1);
    }
    
    
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
    {
      perror("ERROR reading from socket");
      exit(1);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    
    stat( std::chrono::duration_cast < std::chrono::microseconds>( finish - start ).count() );
    
    shutdown(sockfd, SHUT_RDWR);
    
    struct linger l = { 1, 0 };
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &l, sizeof(struct linger));
    close(sockfd);
  }
  */
  return 0;
}


int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    std::cerr << "usage: " << argv[0] << " <addr> <port> <text>" << std::endl;
    exit(1);
  }
  counters.reserve(COUNTER_MAX);
  int threads = 0;
  if ( argc > 4 )
    threads = atoi(argv[4]);
  
  if ( threads == 0)
  {
    run_client(argc, argv);
  }
  else {
    for (int i =0; i < threads; ++i)
    {
      std::thread( std::bind(run_client, argc, argv) ).detach();
    }
    sleep( 3600 );
  }
}
/*
int main(int argc, char* argv[])
{

  if (argc < 4)
  {
    std::cerr << "usage: " << argv[0] << " <addr> <port> <text>" << std::endl;
    exit(1);
  }

  sockaddr_in cliaddr = inet::create_address( argv[1], atoi(argv[2]) );

  std::string str = argv[3];
  str+="\r\n";
  
  char buff[BUFSIZE]={0};
  //size_t bufflen = std::strlen(argv[3]);
  size_t bufflen = str.size();
  unsigned int counter = 0;

  auto start = std::chrono::high_resolution_clock::now();
 
  while(1)
  {
    int connfd = inet::socket(SOCK_STREAM);
    inet::reuseaddr(connfd);
    inet::connect(connfd, cliaddr);
    inet::reuseaddr(connfd);
    if ( inet::send(connfd, str.c_str(), bufflen )
         != inet::recv(connfd, buff, BUFSIZE)
       )
    inet::close(connfd);

    ++counter;
    if ( counter == 10000)
    {
      auto finish = std::chrono::high_resolution_clock::now();
      unsigned long millispan = std::chrono::duration_cast < std::chrono::milliseconds>( finish - start ).count();
      std::cout << millispan << " " << long( ( (1.0/millispan)*1000L) * 10000L ) << std::endl;
      start=finish;
      counter = 0;
    }
  }
}
*/