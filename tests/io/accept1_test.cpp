#include <iostream>


#include <wfc/io/acceptor/acceptor.hpp>
#include <wfc/io/strategy/ip/tcp/acceptor/async.hpp>
#include <boost/asio.hpp>
#include <string>
#include <thread>

#include <wfc/io/connection/connection.hpp>
#include <wfc/io/strategy/posix/writer/config.hpp>
#include <wfc/io/strategy/posix/connection/rn/stream.hpp>
#include <boost/asio.hpp>

#include <set>
/// connection

struct config
{
    size_t output_buffer_size = 1024;
    size_t input_buffer_size = 1024;
    std::function<void()> not_alive = nullptr;
  std::function<void()> release_function = nullptr;

};


struct ad_reverse
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    std::reverse( d->begin(), d->end() );
    t.get_aspect().template get<wfc::io::_outgoing_>()(t, std::move(d) );
    t.descriptor().close();
  }
};

  
struct connection_aspect: 
  fas::aspect<
    fas::advice< wfc::io::_options_type_, config>,
    fas::advice< wfc::io::_incoming_, ad_reverse>,
    fas::type< wfc::io::_descriptor_type_, boost::asio::ip::tcp::socket>,
    wfc::io::strategy::posix::connection::rn::stream
  >
{
};

typedef wfc::io::connection::connection<connection_aspect> connection_type;

///////////////////

struct _config_;
struct _holder_storage_;
struct _insert_;
struct _holder_type_;


struct ad_insert
{
  template<typename T>
  void operator()(T& t, typename T::descriptor_ptr d)
  {
    typedef typename T::holder_type holder_type;
    //auto& config = t.get_aspect().template get<_config_>();
    auto holder = std::make_unique<holder_type>( std::move(*d) );
    t.configure( *holder);
    holder->start();
    t.get_aspect().template get<_holder_storage_>().insert( std::move(holder) );
  }
};


///////////////////

struct aspect: fas::aspect<
  fas::advice< wfc::io::_options_type_, config>,
  fas::type<  _holder_type_,         connection_type >,
  fas::value< _config_,              config   >,
  fas::advice< wfc::io::_options_type_, config>,
  fas::value< _holder_storage_,      std::set< std::unique_ptr<connection_type> >   >,
  fas::advice< _insert_, ad_insert>
>{};

template<typename A>
class descriptor_manager
  : public fas::aspect_class< A >
{
  typedef descriptor_manager<A> self;
  typedef fas::aspect_class< A > super;
  
public:
  
  // typedef typename super::aspect template advice_cast<_descriptor_type_>::type descriptor_type;
  typedef typename super::aspect:: template advice_cast<_holder_type_>::type holder_type;
  typedef typename holder_type::descriptor_type descriptor_type;
  typedef typename std::unique_ptr<descriptor_type> descriptor_ptr;
  typedef typename super::aspect:: template advice_cast<_config_>::type config_type;
  
public:
  
  std::function<void(holder_type&)> configure;
  
  descriptor_manager(/*const config_type& conf*/)
  {
    // this->get_aspect().template get<_config_>() = conf;
  }
  
  void insert(descriptor_ptr d)
  {
    this->get_aspect().template get<_insert_>()( *this, std::move(d) );
  }
  
private:
  
};

///
/// 
///
typedef descriptor_manager<aspect> manager;

struct ad_ready
{
  std::unique_ptr<manager> manager_ptr;
  template<typename T>
  void operator()( T& , typename T::data_ptr d)
  {
    std::cout << "NEW SOKET" << std::endl;
    manager_ptr->insert(std::move(d));
  }
};

  
struct acceptor_aspect: 
  fas::aspect<
    fas::advice< wfc::io::_options_type_, config>,
    fas::advice< wfc::io::_incoming_, ad_ready>,
    wfc::io::strategy::ip::tcp::acceptor::async
  >
{};

typedef wfc::io::acceptor::acceptor<acceptor_aspect> acceptor_type;


int main()
{
  boost::asio::io_service io_service;
  boost::asio::io_service::work wrk(io_service);
  
  typedef acceptor_type::descriptor_type descriptor_type;
  
  descriptor_type desc( io_service );
  
    boost::asio::ip::tcp::resolver resolver( io_service );
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({
      "0.0.0.0", 
      "12345"
    });

    desc.open(endpoint.protocol());
    desc.set_option( boost::asio::ip::tcp::acceptor::reuse_address(true) );
    desc.bind(endpoint);
    desc.listen();

  
  config conf;
  acceptor_type acceptor( std::move(desc), conf );
  
  
  config conn_config;
  auto mgr = std::make_unique<manager>();
  mgr->configure = [&conn_config](manager::holder_type& holder)
  {
    holder.configure(conn_config);
  };
  acceptor.get_aspect().get<wfc::io::_incoming_>().manager_ptr = std::move(mgr);
  
  acceptor.start();
  io_service.run();
  /*
  std::cout << "-1-" << std::endl;
  ::unlink("/tmp/my_cool_app"); 
  boost::asio::local::stream_protocol::socket sock1(io_service);
  std::thread th( [&io_service, &sock1]()
  {
    std::cout << "-2-" << std::endl;
    boost::asio::local::stream_protocol::endpoint ep1("/tmp/my_cool_app");
    sock1.connect(ep1);
    std::cout << "-2.1-" << std::endl;
    io_service.run();
  });

  boost::asio::local::stream_protocol::endpoint ep("/tmp/my_cool_app");
  boost::asio::local::stream_protocol::acceptor acceptor(io_service, ep);
  boost::asio::local::stream_protocol::socket sock(io_service);
  acceptor.accept(sock);
  
  std::cout << "-3-" << std::endl;
  
  config conf;
  
  connection_type conn( std::move(sock), conf );
  conn.start();
  
   
  std::cout << "-4-" << std::endl;
  std::string income_string = "Hello World\r\n";
  sock1.write_some( boost::asio::buffer(income_string));
  
  std::cout << "-5-" << std::endl;
  std::vector<char> result_data;
  result_data.resize(100);
  size_t size = sock1.read_some( boost::asio::buffer(result_data, 100) );
  
  std::cout << "-6-" << std::endl;
  
  std::string result_string(result_data.begin(), result_data.end());
  
  std::cout << size << "Done! [" << result_string << "]" << std::endl;
  io_service.stop();
  th.join();
  std::cout << size << "Final!" << std::endl;
  */
}

