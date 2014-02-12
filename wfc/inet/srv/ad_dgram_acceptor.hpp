#pragma once

#include <wfc/inet/tags.hpp>
#include <wfc/inet/srv/tags.hpp>
#include <wfc/inet/srv/connection_manager.hpp>
#include <wfc/memory.hpp>
#include <fas/aop/tag.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace wfc{ namespace inet{

struct ad_dgram_acceptor
{
  typedef ::boost::asio::ip::udp::socket socket_type;
  typedef socket_type acceptor_type;
  
  ad_dgram_acceptor()
  {
    std::cout << "ad_dgram_acceptor::ad_dgram_acceptor()" << std::endl;
  }
  
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    std::cout << "dgram operator()" << std::endl;
    
     boost::asio::ip::udp::resolver resolver( t.get_io_service() );
    _endpoint = boost::asio::ip::udp::endpoint(
      *resolver.resolve({
        t.server_context().host, 
        t.server_context().port
      })
    );

    std::cout << "dgram do_accept" << std::endl;
    _acceptor = std::make_shared<acceptor_type>(t.get_io_service(), _endpoint );

    /*
    boost::asio::socket_base::non_blocking_io non_blocking_io(true);
    _acceptor->io_control(non_blocking_io);
    */

    
    do_accept(t);
    
    /*

    _acceptor->open(endpoint.protocol());
    _acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor->bind(endpoint);
    _acceptor->listen();
    
    t.get_aspect().template get<_st_acceptor_>()(t, _acceptor);
    t.get_aspect().template get<_mt_acceptor_>()(t, _acceptor);
    */
  }
  
  template<typename T>
  void do_accept(T& t)
  {
    //typedef typename T::connection_type connection_type;
    typedef typename T::connection_manager_type connection_manager_type;
    
    _acceptor->async_receive_from(
      boost::asio::buffer(_data, max_length), 
      _sender_endpoint,
      [this, &t](boost::system::error_code ec, std::size_t bytes_recvd)
      {
        std::cout << "DGRAM RECV " << _sender_endpoint.port() << "[" << std::string(this->_data, this->_data + bytes_recvd) << "] {{{" << std::endl;
        if (!this->_acceptor->is_open())
          return;

        if (!ec)
        {
          //!!! t.get_aspect().template get<_socket_>()(t, *(this->_acceptor)); 
          //this->_acceptor->connect(this->_sender_endpoint);
          
          data_ptr data = std::make_unique<data_type>( this->_data, this->_data + bytes_recvd );
          
          //auto wdata = unique_wrap( std::move(data) );
          
          //t.get_aspect().template get<_worker_>()(t, std::move( *(this->_acceptor) ), [this, &t, wdata](socket_type sock) -> void
          {
            typedef typename T::connection_type connection_type;
            //std::shared_ptr<connection_manager> manager = t.get_aspect().template get<_connection_manager_>();
            std::shared_ptr<connection_manager_type> manager = t.connection_manager();
            std::shared_ptr<connection_type> conn = manager->find( _sender_endpoint.address(), _sender_endpoint.port() );
            
            if ( conn == nullptr )
            {
              std::cout << "NEW CONNECTION" << std::endl;
              std::shared_ptr<connection_type> pconn = t.create_connection( 
                this->_acceptor,
                this->_sender_endpoint,
                [&t](std::shared_ptr<connection_type> pconn)->void
                {
                  //t.get_aspect().template get<_connection_manager_>()->erase(pconn);
                  t.connection_manager()->erase(pconn);
                }
              );
              //pconn->context().activity = manager;
              manager->insert(pconn, this->_sender_endpoint.address(), this->_sender_endpoint.port());
              pconn->start();
              conn = pconn;
            }
            else
              std::cout << "EXIST CONNECTION" << std::endl;
            
            
            std::cout << "FOUNDED " << conn->remote_port() << "(" << size_t( conn.get() ) << ")"<< ": " << std::string(data->begin(), data->end()) << std::endl;
            
            //auto d = unique_unwrap(wdata);
            conn->on_read(std::move(data) );
            std::cout << "}}} DGRAM RECV " << std::endl;
            
            
          }//);
        }
        else
          std::cout <<  "single do accept error " <<  ec.message() <<  std::endl;

        this->do_accept(t);

      }
    );
  }
  
  template<typename T>
  void operator()(T& /*t*/, fas::tag<_stop_>)
  {
    _acceptor->close();
    /*
     * t.get_aspect().template get<_st_acceptor_>()(t);
    t.get_aspect().template get<_mt_acceptor_>()(t);
    */
  }

  template<typename T>
  void operator()(T& /*t*/, fas::tag<_reconfigure_>)
  {
    // TODO: мягкий перезапуск
  }

private:
  std::shared_ptr< acceptor_type > _acceptor;
  boost::asio::ip::udp::endpoint _endpoint;
  boost::asio::ip::udp::endpoint _sender_endpoint;
  
  enum { max_length = 1024 };
  char _data[max_length];
};

}}