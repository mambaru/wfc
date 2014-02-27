#pragma once

#include "connection.hpp"

#include <wfc/io/acceptor/acceptor.hpp>
#include <wfc/io/strategy/ip/tcp/acceptor/async.hpp>
#include <wfc/io/server/tags.hpp>
#include <fas/aop.hpp>

#include <set>



namespace wfc{ namespace io{ namespace ip{ namespace tcp{ namespace rn{ namespace impl{ 

///////////////////

struct _config_;
struct _holder_storage_;
struct _insert_;
struct _holder_type_;

struct holder_config
{
  size_t output_buffer_size = 1024;
  size_t input_buffer_size = 1024;
  std::function<void()> not_alive = nullptr;
  std::function<void()> release_function = nullptr;
};

struct ad_insert
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::aspect::template advice_cast<_holder_type_>::type holder_type;
    //auto& config = t.get_aspect().template get<_config_>();
    
    auto holder = std::make_unique<holder_type>( std::move(*d), holder_config() /*!!!!*/);
    //t.configure( *holder);
    holder->start();
    t.get_aspect().template get<_holder_storage_>().insert( std::move(holder) );
  }
};

///////////////////

struct connection_manager_aspect: fas::aspect<
  fas::type<  _holder_type_,         connection >,
  fas::value< _config_,              holder_config   >,
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
    //std::cout << "NEW SOKET" << std::endl;
    t.get_aspect().template get<_insert_>()(t, std::move(d));
    //manager_ptr->insert(std::move(d));
  }
};

  
struct acceptor_aspect: 
  fas::aspect<
    connection_manager_aspect,
    fas::advice< wfc::io::_incoming_, ad_ready>,
    wfc::io::strategy::ip::tcp::acceptor::async
  >
{};

  
class acceptor:
  public wfc::io::acceptor::acceptor<acceptor_aspect>
{
  typedef wfc::io::acceptor::acceptor<acceptor_aspect> super;
public:
  template<typename Conf>
  acceptor(descriptor_type&& desc, const Conf& conf)
    : super( std::move(desc), conf )
  {
  }
};
  
}}}}}}