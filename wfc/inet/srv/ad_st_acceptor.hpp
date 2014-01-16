#pragma once

#include <wfc/inet/srv/connection_manager.hpp>
#include <wfc/inet/tags.hpp>
#include <wfc/memory.hpp>

#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace inet{

struct ad_st_acceptor
{
  typedef ::boost::asio::ip::tcp::socket socket_type;
  typedef ::boost::asio::ip::tcp::acceptor acceptor_type;
  
  ad_st_acceptor()
    //: _connection_manager( std::make_shared<connection_manager>() )
  {}
  template<typename T>
  void operator()(T& /*t*/)
  {
    auto acceptor = _acceptor.lock();
    if ( !acceptor )
      return;
    acceptor->close();
  }
  
  template<typename T>
  void operator()(T& t, std::weak_ptr<acceptor_type> acceptor)
  {
    int threads = t.server_context().listen_threads;
    if ( threads == 0 )
    {
      //bool empty = !_acceptor.lock();
      bool equal = !_acceptor.owner_before(acceptor) && !acceptor.owner_before(_acceptor);
      if ( /*empty ||*/ !equal )
      {
        if ( auto acc = _acceptor.lock() )
            acc->close();
        _acceptor = acceptor;
        _socket = std::make_unique<socket_type>(t.get_io_service());
        this->do_accept(t);
      }
    }
    else
    {
      if ( auto acceptor = _acceptor.lock() )
        acceptor->close();
      
      _acceptor.reset();
    }
  }
  
  template<typename T>
  void do_accept(T& t)
  {
    std::weak_ptr<acceptor_type> acceptor_weak = _acceptor;
    auto acceptor = acceptor_weak.lock();
    if ( !acceptor )
      return;
    acceptor->async_accept(
      *_socket,
      [this, &t, acceptor_weak](boost::system::error_code ec)
      {
        auto acceptor = acceptor_weak.lock();
        if ( !acceptor )
          return;

        if (!acceptor->is_open())
          return;

        if (!ec)
        {
          t.get_aspect().template get<_socket_>()(t, *(this->_socket)); 
          t.get_aspect().template get<_worker_>()(t, std::move( *(this->_socket) ), [this, &t](socket_type sock) -> void
          {
            typedef typename T::connection_type connection_type;
            std::shared_ptr<connection_manager> manager = t.get_aspect().template get<_connection_manager_>();
            
            std::shared_ptr<connection_type> pconn = t.create_connection( 
              std::move( sock ), 
              [&t](std::shared_ptr<connection_type> pconn)->void
              {
                t.get_aspect().template get<_connection_manager_>()->erase(pconn);
                //this->_connection_manager->erase(pconn);
              }
            );
            
            pconn->context().activity = manager;

            /*
            std::shared_ptr<connection_type> pconn = std::make_shared<connection_type>();
            pconn->context() = t.connection_context();
            */
            manager->insert(pconn);
            pconn->start();
          });
        }
        else
          std::cout <<  "single do accept error " <<  ec.message() <<  std::endl;

        this->do_accept(t);
      } // callback
    ); // async_accept
  }


private:
  std::unique_ptr<socket_type> _socket;
  std::weak_ptr<acceptor_type> _acceptor;
  //std::shared_ptr<connection_manager> _connection_manager;
};


}}