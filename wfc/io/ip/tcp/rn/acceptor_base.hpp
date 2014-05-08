#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_aspect.hpp>
#include <wfc/io/acceptor/acceptor.hpp>

#include <thread>
namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

template<typename A = fas::aspect<> >
class acceptor_base:
  public wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type >
{
  typedef wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  acceptor_base(descriptor_type&& desc, const options_type& conf/*, wfc::io::incoming_handler handler = nullptr*/)
    : super( std::move(desc), conf/*, handler*/)
  {
  }
  
  ~acceptor_base()
  {
  }
  
  void stop(std::function<void()> finalize)
  {
    std::atomic<bool> flag(false);
    super::stop([this, finalize, &flag](){
      
      auto &stg = this->get_aspect().template get<_holder_storage_>();
      for(auto& conn : stg)
      {
        conn.second->stop(nullptr);
      }
      
      stg.clear();

      
      if (finalize!=nullptr)
      {
        finalize();
      }
      
      flag = true;
    });
    
    
    while ( !flag )
    {
      
      super::get_io_service().reset();
      super::get_io_service().poll();
    }
    

  }
};
  
}}}}}
