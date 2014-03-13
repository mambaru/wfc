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
    
    // TODO: Вынести в аспект и инициализировать на этапе старта 
    holder_options_type holder_options = t.options();
    
    /*if ( holder_options.startup_handler != nullptr )
      abort();
      */
    
    auto startup_handler = holder_options.startup_handler;
    
    holder_options.startup_handler = 
      [&t, startup_handler]( ::wfc::io::io_id_t id, ::wfc::io::callback clb, ::wfc::io::add_shutdown_handler add )
    {
      if ( startup_handler != nullptr )
        startup_handler( id, clb, add );
      
      std::cout << "acceptor startup handler " << id << std::endl;
      
      add( [&t](::wfc::io::io_id_t id) 
      {
        std::cout << "accept shutdown " << id << std::endl;
        // post костыль, может не сработать, удаляем объект во время стопа
        t.post([id,&t]()
        {
          std::cout << "close connection id " << id << std::endl;
          t.get_aspect().template get<_holder_storage_>().erase(id);
        });
      });
    };
    
    auto holder = std::make_unique<holder_type>( std::move(*d), holder_options, t._handler );
    auto id = holder->get_id();
    holder->start();
    t.get_aspect().template get<_holder_storage_>().insert( std::make_pair( id, std::move(holder) ) );
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
    //manager_ptr->insert(std::move(d));
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