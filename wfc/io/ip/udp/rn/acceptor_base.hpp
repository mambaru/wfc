#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/udp/rn/acceptor_aspect.hpp>
#include <wfc/io/ip/udp/rn/connection_base.hpp>
#include <wfc/io/acceptor/acceptor.hpp>

#include <thread>
namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ 

template<typename A = fas::aspect<> >
class acceptor_base
  : private connection_base<A>
  //public wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type >
{
  typedef connection_base<A> super;
  //typedef wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type > super;
  typedef typename fas::merge_aspect<A, acceptor_aspect>::type aspect_type; // Костыль
  
public:
  typedef typename super::descriptor_type descriptor_type;
  typedef typename aspect_type::options_type options_type; 
  
  
  acceptor_base(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf.connection)
    , _options( conf )
  {
    //_connection = std::make_shared<connection>(desc, conf.connection);
  }
  
  ~acceptor_base()
  {
  }
  
  void start()
  {
    COMMON_LOG_MESSAGE("start ignore")
  }
  
  void listen()
  {
    COMMON_LOG_MESSAGE("udp \"listen\" " << _options.host << ":" << _options.port)
    
    boost::asio::ip::udp::resolver resolver( super::get_io_service() );
    boost::asio::ip::udp::endpoint endpoint = *resolver.resolve({
      _options.host, 
      _options.port
    });

    super::descriptor().open(endpoint.protocol());
    //super::descriptor().set_option( boost::asio::ip::udp::acceptor::reuse_address(true) );
    super::descriptor().bind(endpoint);
    super::start();
    

    /*
    COMMON_LOG_MESSAGE("listen " << super::options().host << ":" << super::options().port)
    
    boost::asio::ip::udp::resolver resolver( super::get_io_service() );
    boost::asio::ip::udp::endpoint endpoint = *resolver.resolve({
      super::options().host, 
      super::options().port
    });

    super::descriptor().open(endpoint.protocol());
    super::descriptor().set_option( boost::asio::ip::udp::acceptor::reuse_address(true) );
    super::descriptor().bind(endpoint);
    super::descriptor().listen( super::options().backlog );
    */
  }

  
  void stop(std::function<void()> finalize)
  {
    super::stop([this, finalize]()
    {
      /*
      typename super::lock_guard lk( this->mutex() );
      auto &stg = this->get_aspect().template get<_holder_storage_>();
      for(auto& conn : stg)
      {
        conn.second->stop(nullptr);
      }
      
      stg.clear();

      this->mutex().unlock();
      if (finalize!=nullptr)
      {
        finalize();
      }
      this->mutex().lock();
      */
    });
  }
private:
  options_type _options;
  //std::shared_ptr<connection> _connection;
};
  
}}}}}
