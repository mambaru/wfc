#pragma once

#include <comet/asio/connection.hpp>
#include <comet/asio/iconnection.hpp>
#include <comet/asio/server_config.hpp>
#include <boost/asio.hpp>
#include <set>

namespace mamba{ namespace comet{ namespace inet{
  
template<typename Conn=echo_connection<> >
class tcp_server
{
public:
  typedef Conn connection_type;
  typedef std::shared_ptr<connection_type> connection_ptr;
  //typedef typename connection_type::origin_connection origin_connection;
  typedef std::shared_ptr<connection_type> iconnection_ptr;
  typedef boost::asio::ip::tcp::socket socket_type;
  //typedef std::unique_ptr<socket_type> socket_ptr;
  typedef socket_type* socket_ptr;
  
  typedef typename connection_type::context_type context_type;
  
public:
  tcp_server( ::boost::asio::io_service& io_service)
    : _io_service(io_service)
    , _acceptor(io_service)
    // , _socket(nullptr)
    , _socket(io_service)

  {
    
  }
  
  /*
  connection_type& prototype() { return _prototype; }

  const connection_type& prototype() const { return _prototype; }
  */
  context_type& context()
  {
    return _context;
  }

  const context_type& context() const
  {
    return _context;
  }


  void start( const server_config& conf)
  {
    std::cout << conf.host << ":" << conf.port << std::endl;
    boost::asio::ip::tcp::resolver resolver(_io_service);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({conf.host, conf.port});
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    this->do_accept();
    if ( !_acceptor.is_open() )
      std::cout << "dont open!" << std::endl;

  }

  void stop()
  {
    //std::cout << "server::stop _acceptor.close();" << std::endl;
    _acceptor.close();
    //std::cout << "server::stop c->close();" << std::endl;
    //std::vector<connection_ptr> all_conn
    for( auto& c : _connections )
      c->close();
      

    while (!_connections.empty())
    {
      _io_service.poll_one();
      std::cout << "_io_service.poll_one()" << std::endl;
    }
    //std::cout << "server::stop _connections.clear();" << std::endl;
    //_connections.clear();
    //std::cout << "server::stop done!" << std::endl;
    
  }

  void do_accept()
  {
    std::cout << "do_accept()!" << std::endl;
    //_socket = new socket_type(_io_service);
    _acceptor.async_accept(
      _socket,
      [this](boost::system::error_code ec)
      {
        
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!_acceptor.is_open())
        {
          std::cout << "!_acceptor.is_open()" << std::endl;
          //delete _socket;
          return;
        }

        std::cout << "accepted!" << std::endl;

        if (!ec)
        {
          connection_ptr pconn = std::make_shared<connection_type>();
          pconn->context() = _context;
          //connection_ptr pconn( _prototype.clone() );
          /*iconnection_ptr iconn;
          iconn = pconn;*/
          this->_connections.insert(pconn);
          pconn->start( std::move( this->_socket), [this](connection_ptr conn)->void{
            std::cout << "erase connection" << std::endl;
            this->_connections.erase(conn);
            std::cout << "erased!!!" << std::endl;
          });
          /*
          connection_manager_.start(std::make_shared<connection>(
              std::move(socket_), connection_manager_, request_handler_));
          */
        }

        this->do_accept();
      } // callback
    ); // async_accept
  }
  
private:
  ::boost::asio::io_service& _io_service;
  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor _acceptor;
  /// The next socket to be accepted.
  //socket_ptr _socket;
  socket_type _socket;
  std::set<connection_ptr> _connections;
  context_type _context;

};

}}}