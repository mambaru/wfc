#pragma once 

#include <efc/inet/srv/tags.hpp>
#include <boost/asio.hpp>

namespace wfc{ namespace inet{ namespace srv{ namespace ip{ namespace tcp{
 
// TODO: перенести в inet::srv
struct ad_acceptor_start
{
  typedef std::shared_ptr<boost::asio::io_service> io_service_ptr;
  typedef std::shared_ptr<std::thread> thread_ptr;
  typedef std::pair<io_service_ptr, std::thread> acceptor_thread;
  typedef std::list<acceptor_thread> thread_list;
  
public:
  
  template<typename T>
  void operator()(T& t, fas::tag<_start_>)
  {
    typedef typename T::aspect::template advice_cast<_acceptor_type_>::type acceptor_type;
    typedef std::shared_ptr<acceptor_type> acceptor_ptr;
    int threads = t.server_context().listen_threads;
    acceptor_ptr pacc = t.get_aspect().template get<_acceptor_>();
    if (threads == 0)
    {
      do_accept(t, *pacc);
    }
    else
    {
      std::generate_n( std::back_inserter(_threads), threads, [this, &t, pacc]() 
      {
        auto io = std::make_shared<boost::asio::io_service>();
        auto t = std::make_shared<std::thread>([this, &t, io, pacc]() 
        {
          acceptor_type::endpoint_type endpoint = pacc->local_endpoint();
          acceptor_type::native_type fd = ::dup( pacc->native() );
          acceptor_type acceptor(*io);
          acceptor.assign(endpoint.protocol(), fd);
          this->do_accept(t, acceptor);
        });
        
        return std::make_pair(io, t);
      });
    }
  }

  template<typename T, typename Acceptor>
  void do_accept(T& t, Acceptor& acc)
  {
    typedef typename T::socket_type socket_type;
    typedef typename T::connection_type connection_type;
    typedef std::shared_ptr<socket_type> socket_ptr;
    
    socket_ptr sock = std::make_shared< socket_type >( acc.get_io_service() );
    
    _acceptor.async_accept(
      *sock,
      [this, &t, &acc, sock](boost::system::error_code ec)
      {
        if (!this->acc.is_open())
          return;

        // TODO: aborted!!!
        if (!ec)
        {
          t.get_aspect().template get<_socket_control_>()(t, *sock); 
          t.get_aspect().template get<_new_connection_>()(t, sock); 
        }
        
        this->do_accept(t);
      }
    );
    
  }

private:
  
  thread_list _threads;
};

  
}}}}}
