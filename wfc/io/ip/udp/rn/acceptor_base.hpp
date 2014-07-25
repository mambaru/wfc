#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/udp/rn/acceptor_aspect.hpp>
#include <wfc/io/acceptor/acceptor.hpp>

#include <thread>
namespace wfc{ namespace io{ namespace ip{ namespace udp{ namespace rn{ 

template<typename A = fas::aspect<> >
class acceptor_base:
  public wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type >
{
  typedef wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  acceptor_base(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
  }
  
  ~acceptor_base()
  {
  }
  
  void listen()
  {
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
};
  
}}}}}
