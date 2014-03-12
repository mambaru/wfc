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
    
    auto holder = std::make_unique<holder_type>( std::move(*d), t.options(), t._handler );
    holder->start();
    t.get_aspect().template get<_holder_storage_>().insert( std::move(holder) );
  }
};

///////////////////

struct connection_manager_aspect: fas::aspect<
  fas::advice< wfc::io::_options_type_, acceptor_options>,
  fas::type<  _holder_type_,         connection >,
  fas::value< _config_,              acceptor_options  >,
  fas::value< _holder_storage_,      std::set< std::unique_ptr<connection> >   >,
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