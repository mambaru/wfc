#pragma once

#include <wfc/io/ip/tcp/rn/connection.hpp>
#include <wfc/io/ip/tcp/rn/acceptor_options.hpp>
#include <wfc/io/acceptor/acceptor.hpp>
#include <wfc/io/strategy/ip/tcp/acceptor/async.hpp>
#include <wfc/io/server/tags.hpp>
#include <fas/aop.hpp>


#include <set>



namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{

///////////////////

struct _config_;
struct _holder_storage_;
struct _insert_;
struct _holder_type_;

struct ad_insert
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::aspect::template advice_cast<_holder_type_>::type holder_type;
    typedef typename holder_type::options_type holder_options_type;
    typedef std::unique_ptr<holder_type> holder_ptr;
    
    // TODO: Вынести в аспект и инициализировать на этапе старта 
    holder_options_type holder_options = t.options().connection;
    
    /*if ( holder_options.startup_handler != nullptr )
      abort();
      */
    
    auto startup_handler = holder_options.startup_handler;
    
    holder_options.startup_handler = 
      [&t, startup_handler]( ::wfc::io::io_id_t id, ::wfc::io::callback clb, ::wfc::io::add_shutdown_handler add )
    {
      std::cout << "NEW CONNECTON" << std::endl;
      if ( startup_handler != nullptr )
        startup_handler( id, clb, add );
      std::cout << "NEW CONNECTON 2" << std::endl;
      
      add( [&t](::wfc::io::io_id_t id) 
      {
        std::cout << "connection stop handler -1- ----------------------------------- id=" << id << std::endl;
        // post костыль, может не сработать, удаляем объект во время стопа
        t.post([id,&t]()
        {
          std::cout << "connection stop handler -1.1-" << std::endl;
          auto &stg = t.get_aspect().template get<_holder_storage_>();
          std::cout << "connection stop handler -1.1.1-" << std::endl;
          auto itr = stg.find(id);
          std::cout << "connection stop handler -1.1.2-" << std::endl;
          if ( itr != stg.end() )
          {
            std::cout << "connection stop handler -1.2-0-" << std::endl;
            auto pconn = std::make_shared<holder_ptr>( std::move(itr->second) );
            std::cout << "connection stop handler -1.2-1-" << std::endl;
            stg.erase(itr);
            std::cout << "after erase stg.size()=" << stg.size() << std::endl;
            std::cout << "connection stop handler -1.2- stg.size="<< stg.size() << std::endl;
            /*(*pconn)->strand()*/
            t.post([pconn, id](){
              std::cout << "connection stop handler -1.2.1-" << std::endl;
              std::cout << "smart delete " << id << std::endl;
            });
            std::cout << "connection stop handler -1.3-" << std::endl;
            (*pconn)->stop();
            std::cout << "connection stop handler -1.4-" << std::endl;
            //(*pconn).reset();
            //std::cout << "connection stop handler -1.5-" << std::endl;
          }
          else
          {
            std::cout << "ERROR not found io_id=" << id << std::endl; 
          }
          // t.get_aspect().template get<_holder_storage_>().erase(id);
        });
        std::cout << "connection stop handler -2-" << std::endl;
        //t.get_io_service().poll();
        // std::cout << "connection stop handler -3-" << std::endl;
      });
      std::cout << "NEW CONNECTON 3" << std::endl;
    };
    
    auto holder = std::make_unique<holder_type>( std::move(*d), holder_options, t._handler );
    auto id = holder->get_id();
    auto res = t.get_aspect().template get<_holder_storage_>().insert( std::make_pair( id, std::move(holder) ) );
    if ( res.second )
    {
      res.first->second->start();
    }
    else
    {
      abort();
    }
    //holder->start();
    
  }
};

///////////////////

struct connection_manager_aspect: fas::aspect<
  fas::advice< wfc::io::_options_type_, acceptor_options>,
  fas::type<  _holder_type_,         connection >,
  fas::value< _config_,              acceptor_options  >,
  fas::value< _holder_storage_,      std::map< ::wfc::io::io_id_t, std::unique_ptr<connection> >   >,
  fas::advice< _insert_, ad_insert>
>{};

///
  

struct ad_ready
{
  // std::unique_ptr<manager> manager_ptr;
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get<_insert_>()(t, std::move(d));
  }
};

  
struct acceptor_aspect: 
  fas::aspect<
    connection_manager_aspect,
    fas::advice< wfc::io::_incoming_, ad_ready>,
    wfc::io::strategy::ip::tcp::acceptor::async,
    
    // Доработать
    fas::stub< wfc::io::_create_ >,
    fas::stub< wfc::io::_start_ >,
    fas::stub< wfc::io::_stop_ >

  >
{};

}}}}}