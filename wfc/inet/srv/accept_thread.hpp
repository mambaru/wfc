#pragma once

#include <wfc/inet/srv/connection_manager.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <memory>

namespace wfc{ namespace inet{
 
class accept_thread
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;

public:

  accept_thread(const accept_thread&) = delete;
  accept_thread& operator=(const accept_thread&) = delete;
  
  accept_thread(::boost::asio::io_service& io)
    : _work(io)
    , _socket(io)
    , _acceptor()
  {
  }
  
  template<typename T>
  void start(T& t, std::weak_ptr<acceptor_type> acceptor)
  {
    _acceptor = acceptor;
    typedef typename T::connection_type connection_type;
    
    _thread = std::thread([this, &t]()
    {
      connection_manager mgr;
      
      for(;;)
      {
        auto acceptor = this->_acceptor.lock();
        if ( !acceptor )
          break;
        
        boost::system::error_code ec;
        acceptor->accept( this->_socket, ec);
        
        if (!acceptor->is_open())
          break;
        
        if (!ec)
        {
          t.get_aspect().template get<_socket_>()(t, this->_socket); 
          t.get_aspect().template get<_worker_>()(t, std::move( this->_socket), [&t, &mgr](socket_type sock) 
          {
            std::shared_ptr<connection_type> pconn = t.create_connection();
            mgr.insert(pconn);
            pconn->start( std::move( sock ), [&mgr](std::shared_ptr<connection_type> pconn)->void
            {
              mgr.erase(pconn);
            });
          });
        }
        else
          std::cout << "thread accept error: " << ec.message() << std::endl;
          
      }
    });
    //sleep(1);
  }

private:  
  ::boost::asio::io_service::work _work;
  socket_type _socket;
  std::weak_ptr<acceptor_type> _acceptor;
  std::thread _thread;
};


}}