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
  
  acceptor_base(descriptor_type&& desc, const options_type& conf, wfc::io::handler handler = nullptr)
    : super( std::move(desc), conf, handler)
  {
  }
  
  ~acceptor_base()
  {
  }
  
  void stop(std::function<void()> finalize)
  {
    std::atomic<bool> flag(false);
    super::stop([this, finalize, &flag](){
      std::cout << "acceptor_base::stop" << std::endl;
      auto &stg = this->get_aspect().template get<_holder_storage_>();
      for(auto& conn : stg)
      {
        std::cout << "acceptor_base::stop connection stop..." << std::endl;
        conn.second->stop(nullptr);
        std::cout << "acceptor_base::stop connection stop... Done" << std::endl;
      }
      
      stg.clear();
      
    
      /*
      bool tmp = false;
      std::thread th([&tmp, this](){
        wfc::io_service::strand& strand = this->strand();
        while(!tmp)
        {
          std::cout << "&strand=" << size_t(&strand)<< std::endl;
          strand.post([&strand](){
            std::cout << "----thread test---" << size_t(&strand)<< std::endl;
          });
          usleep(1000000);
        }
      });
      
      std::atomic<int> counter(0);
      while ( !stg.empty() )
      {
        int c = ++counter;
        this->get_io_service().post( this->strand().wrap([c](){
          std::cout << "----wait test--- " << c << std::endl;
        }));
        std::cout << "acceptor_base::stop wait empty " << stg.size() << " this="<< size_t(this) << std::endl;
        this->get_io_service().reset();
        this->get_io_service().poll();
        usleep(1000000);
      }
      
      tmp = true;
      th.join();
      */
      
      if (finalize!=nullptr)
      {
        finalize();
      }
      std::cout << "acceptor_base::stop Done" << std::endl;
      flag = true;
    });
    
    
    while ( !flag )
    {
      std::cout << "acceptor_base poll" << std::endl;
      super::get_io_service().reset();
      super::get_io_service().poll();
    }
    
    std::cout << "acceptor_base::stop" << std::endl;
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
