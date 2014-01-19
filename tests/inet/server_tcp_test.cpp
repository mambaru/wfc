#include <wfc/inet/server.hpp>
#include <wfc/inet/echo/aspect_stream_rn.hpp>
#include <wfc/inet/client/aspect_client_tcp.hpp>

#include <wfc/inet/client/client_aspect.hpp>

#include <boost/asio.hpp>
#include <thread>
#include <chrono>

namespace wfc{ namespace inet{

struct aspect_client_default
: fas::aspect< 
    connection_aspect< echo::aspect_stream_rn >,
    client_aspect< aspect_client_tcp >,
    connection_base_class< fas::aspect_class >,
    connection_class< dgram_connection >
  > 
{
};


template<typename... Args>
struct client_helper
{
  typedef typename fas::merge_aspect< fas::aspect<Args...>, aspect_client_default>::type aspect_type;
  typedef typename fas::aspect_class<aspect_type>::aspect aspect;
  
  ///
  /// connection
  ///
  typedef typename aspect::template advice_cast<_connection_aspect_>::type connection_aspect_type;
  
  template<typename ConnAspect>
  using connection_base_t = typename aspect::template advice_cast<_connection_base_class_>::type::template type<ConnAspect>;
  
  template<typename ConnAspect, template<typename> class ConnBase >
  using connection_t = typename aspect::template advice_cast<_connection_class_>::type::template type<ConnAspect, ConnBase>;
  
  typedef connection_t<connection_aspect_type, connection_base_t> connection_type;
  typedef typename connection_type::aspect::template advice_cast<_context_>::type connection_context_type;
  typedef typename connection_type::aspect::template advice_cast<_socket_type_>::type socket_type;
  
  ///
  /// client
  ///
  typedef typename aspect::template advice_cast<_client_aspect_>::type client_aspect_type;

  typedef fas::aspect_class<client_aspect_type> client_base;
  typedef typename client_base::aspect::template advice_cast<_context_>::type client_context_type;
  typedef typename client_base::aspect::template advice_cast<_configuration_>::type::config_type config_type;
  

};



  
//template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >

template<typename... Args>
class client
  : public client_helper<Args...>::client_base
{
  typedef client_helper<Args...> helper;
public:
  typedef typename helper::client_base super;
  typedef typename helper::connection_type connection_type;
  typedef typename helper::socket_type  socket_type;
  typedef typename helper::connection_context_type connection_context_type;
  typedef typename helper::client_context_type client_context_type;
  typedef typename helper::config_type config_type;
  typedef connection_manager<> connection_manager_type;

  /*
  client(::boost::asio::io_service& io)
    : _socket( std::make_shared<socket_type>(io) )
  {
  }
  */
  
  client( ::wfc::io_service& io_service, const config_type& conf )
    : _io_service(io_service)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  client( std::weak_ptr< wfc::global > g, const config_type& conf )
    : client( *(g.lock()->io_service.lock()), conf)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  void reconfigure(const config_type& conf)
  {
    this->get_aspect().template get<_configuration_>()(*this, conf);
  }
  
  void initialize()
  {
  }
  
  void start()
  {
    this->get_aspect().template getg<_startup_>()(*this, fas::tag<_startup_>() );
    this->get_aspect().template getg<_start_>()(*this, fas::tag<_start_>() );
  }
  
  void stop()
  {
    this->get_aspect().template getg<_stop_>()(*this, fas::tag<_stop_>() );
  }
  
  client_context_type client_context() const
  {
    return this->get_aspect().template get<_context_>();
  }
  
  void client_context(const client_context_type& value) 
  {
    this->get_aspect().template get<_context_>() = value;
  }

  connection_context_type connection_context() const
  {
    return _connection_context;
  }
  
  void connection_context(const connection_context_type& value)
  {
    _connection_context = value;
  }
  
  boost::asio::io_service& get_io_service()
  {
    return _io_service;
  }
  
  virtual void send(data_ptr d) 
  {
    this->get_aspect().template get<_connection_manager_>()->least()->send( std::move(d) );
  }
  
  template< typename ...ConnArgs>
  std::shared_ptr<connection_type> create_connection( ConnArgs&& ...args )
  {
    std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>( std::forward<ConnArgs>(args)... );
    pconn->context() = _connection_context;
    return pconn;
  }
  
private:
  ::wfc::io_service& _io_service;
  connection_context_type _connection_context;

  
  //std::unique_ptr<connection_type> _connection;
};

}}


struct common_aspect: 
  fas::aspect< 
    wfc::inet::connection_aspect< wfc::inet::echo::aspect_stream_rn >
  > 
{
};

::boost::asio::io_service io_service;

std::string str1 = "1234567890\r\nqwertyuiop\r\n1234567890XYZ\r\nйцукен\r\n";
int receiver_count = 0;
void client_receiver(::boost::asio::ip::tcp::socket& s)
{
  ++receiver_count;
  std::thread([&s]()
  {
    std::string str2;
    while ( str2.size() < str1.size() )
    {
      char reply[1024];
      size_t reply_length = s.receive( boost::asio::buffer(reply, 1024) );
      str2+= std::string(reply, reply+ reply_length);
      std::cout << ">>>>>>[[" << str2 << "]]<<<<<<<<<<" << std::endl;
    }
    std::cout << str1 << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << str2 << std::endl;
    if (str1!=str2)
    {
      std::cout << "fuck" << std::endl; 
      abort();
    }
    --receiver_count;
    if ( receiver_count == 0 )
    {
      std::cout << "SERVICE STOP" << std::endl;
      ::io_service.stop();   
    }
    else
      std::cout << "exit without stop" << std::endl;
  }).detach();
}



void client_test()
{
  std::thread([](){
    ::boost::asio::io_service io_service;
    std::cout << "1 client " << std::endl;
    ::boost::asio::ip::tcp::socket s(io_service);
    ::boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::connect(s, resolver.resolve({"0.0.0.0", "12345"}));
    client_receiver(s);
    sleep(1);
    std::cout << "2 client " << std::endl;
    
    //std::string str1 = "1234567890\r\nqwertyuiop\r\n1234567890XYZ\r\nйцукен\r\n";
    auto beg = str1.begin();
    auto end = str1.end();
    while (beg!=end)
    {
      std::cout << "3 client " << std::endl;
      auto cur = beg;
      if ( std::distance(beg, end) < 1 )
        cur = end;
      else
        cur += 1;
      std::this_thread::sleep_for(std::chrono::milliseconds(0));
      std::vector<char> arr(  beg, cur );
      std::cout << "SEND [[" << std::string(arr.begin(), arr.end() ) << std::endl;
      size_t sended = s.send(boost::asio::buffer(arr, arr.size()));
      if ( sended!= arr.size())
      {
        std::cout << "SEND ERROR" << std::endl;
        abort();
      }
      //boost::asio::send(s, boost::asio::buffer(arr, arr.size() ));
      //boost::asio::write(s, boost::asio::buffer(str1, str1.size() ));
      //break;
      beg = cur;
    }
    
    while(receiver_count)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    

    /*
    char reply[1024];
    std::cout << "4 client " << std::endl;
    size_t reply_length = s.receive( boost::asio::buffer(reply, str1.size()) );
    s.close();
    std::cout << "5 client " << std::endl;
    std::string str2( reply, reply + reply_length);
    std::cout << str1 << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << str2 << std::endl;
    ::io_service.stop();
    */

  }).detach();
}


struct ad_test
{
  std::string str;
  int pack_count;
  ad_test()
    : pack_count(0)
  {
    ++receiver_count;
  }
  
  ~ad_test()
  {
    --receiver_count;
  }
  
  template<typename T>
  void operator()(T& t, wfc::inet::data_ptr d )
  {
    str+=std::string(d->begin(), d->end() );
    str+="\r\n";
    std::cout << "CLIENT [" << str << "]" << std::endl;
    ++pack_count;
    if ( pack_count == 4)
    {
      if ( str == str1 )
      {
        t.close();
        t.context().ready = true;
      }
      else
      {
        abort();
      }
    }
    
  }
};

struct test_context
{
  bool ready = false;
};

struct test_aspect: fas::aspect<
  wfc::inet::context<test_context>,
  fas::advice< wfc::inet::rn::_incoming_, ad_test >
>{};

struct client_common_aspect: 
  fas::aspect< 
    wfc::inet::connection_aspect< 
      fas::merge_aspect<
        test_aspect,
        wfc::inet::echo::aspect_stream_rn 
      >::type
    >
  > 
{
};



typedef wfc::inet::client< client_common_aspect > client;
/*
class client
  : public wfc::inet::client< wfc::inet::echo::aspect_stream_rn >
{
  typedef 
public:
  client(::boost::asio::io_service& io)
  
};
*/


void client_receiver2()
{
  
}

void client_test2()
{
  std::thread([]()
  {
    boost::asio::io_service io;
    wfc::inet::client_tcp_config conf;
    conf.host="0.0.0.0";
    conf.port="12345";
    client cli(io, conf);
    std::cout << "---------------- client_test2() start ----------------------------------" << std::endl;
    cli.start();
    std::cout << "---------------- client_test2() run ----------------------------------" << std::endl;
    
    ::boost::asio::io_service::work work(io);
    std::thread thr([&io](){
      io.run();
      std::cout << "---------------- client_test2() FINAL ----------------------------------" << std::endl;
    });
    
    
    auto beg = str1.begin();
    auto end = str1.end();
    while (beg!=end)
    {
      auto cur = beg;
      if ( std::distance(beg, end) < 1 )
        cur = end;
      else
        cur += 1;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      cli.send( std::make_unique< wfc::inet::data_type >( beg, cur ) );
      beg = cur;
    }
    
    //++receiver_count;
    while( cli.connection_context().ready == false  )
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "exit" << std::endl;
    io.stop();
    thr.join();

    
  }).detach();
}


int main(int argc, char */*argv*/[])
{
  wfc::inet::server< common_aspect > srv(io_service, wfc::inet::server_tcp_config() );
  auto conn_conf = srv.connection_context();
  conn_conf.enable_stat = true;
  srv.connection_context(conn_conf);
  
  auto config = srv.server_context();
  config.host = "0.0.0.0";
  config.port = "12345";
// BUG:
// TODO:
  // config.listen_threads = 3;
  config.worker_threads = 3;
  srv.server_context(config);
  srv.start();
  if ( argc==1)
  {
    
    client_test();
    // client_test();
    // client_test();
    
    /*
    client_test2();
    client_test2();
    client_test2();
    */
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //for (;;)
  std::cout << "run" << std::endl;
  std::thread([&srv](){
    while (receiver_count)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "TEST STOP" << std::endl;
    srv.stop();
    io_service.run_one();
    io_service.stop();
  }).detach();
  io_service.run();
  
  std::cout << "FINAL" << std::endl;
  std::cin.get();
  srv.stop();
  
}