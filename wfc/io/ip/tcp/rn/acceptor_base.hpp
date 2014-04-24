#pragma once

#include <fas/aop.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_aspect.hpp>
#include <wfc/io/acceptor/acceptor.hpp>

namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ 

template<typename A = fas::aspect<> >
class acceptor_base:
  public wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type >
{
  typedef wfc::io::acceptor::acceptor< typename fas::merge_aspect<A, acceptor_aspect>::type > super;
public:
  typedef typename super::options_type options_type; 
  typedef typename super::descriptor_type descriptor_type;
  
  acceptor_base(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr)
    : super( std::move(desc), conf, handler)
  {
  }
  
  ~acceptor_base()
  {
  }
  
  void stop(std::function<void()> finalize)
  {
    super::stop([this, finalize](){
      std::cout << "acceptor_base::stop" << std::endl;
      this->get_io_service().reset();
      auto &stg = this->get_aspect().template get<_holder_storage_>();
      for(auto& conn : stg)
      {
        std::cout << "acceptor_base::stop connection stop..." << std::endl;
        conn.second->stop(nullptr);
        std::cout << "acceptor_base::stop connection stop... Done" << std::endl;
      }
      
      while ( !stg.empty() )
      {
        std::cout << "acceptor_base::stop wait empty " << stg.size() << std::endl;
        this->get_io_service().poll();
        usleep(1000000);
      }
      
      if (finalize!=nullptr)
      {
        finalize();
      }
      std::cout << "acceptor_base::stop Done" << std::endl;
    });
    /*
    std::cout << "acceptor_base::stop" << std::endl;
    
    super::get_io_service().reset();
    super::descriptor().cancel();
    
    auto &stg = super::get_aspect().template get<_holder_storage_>();
    for(auto& conn : stg)
    {
      conn.second->stop(nullptr);
    }
    
    super::stop(finalize);
    
    while ( !stg.empty() )
    {
      super::get_io_service().poll();
      usleep(100);
    }
    
    std::cout << "acceptor_base::stop Done" << std::endl;
    */

  }
};
  
}}}}}
