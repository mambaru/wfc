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
  
  void stop()
  {
    std::cout << "void acceptor_base::stop() -1-" << std::endl;
    super::post([this](){
      std::cout << "void acceptor_base::stop() post" << std::endl;
      auto &stg = this->get_aspect().template get<_holder_storage_>();
      for(auto& conn : stg)
      {
        std::cout << "void acceptor_base::stop() post -1- --------------------------------- connection stop" << std::endl;
        conn.second->stop();
        std::cout << "void acceptor_base::stop() post -1.s-" << std::endl;
        conn.second.reset();
      }
      std::cout << "void acceptor_base::stop() post -2-" << std::endl;
      stg.clear();
      std::cout << "void acceptor_base::stop() post -3-" << std::endl;
    });
    //super::get_io_service().poll();
    std::cout << "void acceptor_base::stop() -2-" << std::endl;
    super::stop();
    std::cout << "void acceptor_base::stop() -3-" << std::endl;
    
    /*
    std::cout << "void acceptor_base::stop() ------ " << std::endl;
    _impl->stop();
    std::cout << "void acceptor::stop() reset..." << std::endl;
    auto &stg = _impl->get_aspect().get<_holder_storage_>();
    for(auto& conn : stg)
    {
      std::cout << "void acceptor::stop() -1- --------------------------------- connection stop" << std::endl;
      conn.second->stop();
      std::cout << "void acceptor::stop() -1.s-" << std::endl;
      conn.second.reset();
    }
    std::cout << "void acceptor::stop() -2-" << std::endl;
    stg.clear();
    std::cout << "void acceptor::stop() -3-" << std::endl;
    _impl.reset();
    std::cout << "...void acceptor::stop()" << std::endl;
    */

  }
};
  
}}}}}
