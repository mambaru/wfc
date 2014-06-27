#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/types.hpp>

#include <wfc/io/reader/aspect/aspect.hpp>

#include <wfc/io/basic/aspect.hpp>
#include <wfc/io/common/fork.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <wfc/logger.hpp>
#include <wfc/callback/callback_status.hpp>



#include <list>
#include <atomic>
#include <chrono>

namespace wfc{ namespace io{ namespace reader{ 


struct options
  : basic_options
  ,  ::wfc::io::basic::options
{
  // std::function<callback_status()> not_alive = nullptr;
};

typedef std::list<  ::wfc::io::data_ptr> data_list;
typedef std::list<  ::wfc::io::outgoing_handler_t> callback_list;
struct _sync_read_some_;
struct _sync_read_more_;
struct _read_incoming_;
struct _callback_list_;
struct _outgoing_list_;

template<typename TgAsyncReadMore>
struct read_incoming
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    if ( d!=nullptr)
      lst.push_back( std::move(d) );
    
    auto& clb = t.get_aspect().template get<_callback_list_>();
    while ( !clb.empty() &&  !lst.empty() )
    {
      auto d2 = std::move( lst.front() );
      lst.pop_front();
      auto handler = clb.front();
      clb.pop_front();
      if ( handler != nullptr )
        handler( std::move(d2) );
    }
    
    if ( !clb.empty() )
      t.get_aspect().template get< TgAsyncReadMore >()(t);

  }
};

struct _read_;
template<typename TgReadMore>
struct read
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    if ( !t.status() )
    {
      return nullptr;
    }
    
    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    if ( lst.empty() && t.status() )
      t.get_aspect().template get< TgReadMore >()(t);

    if ( !t.status() )
    {
      return nullptr;
    }

    
    typename T::data_ptr d = nullptr;
    if ( !lst.empty() )
    {
      d = std::move( lst.front() );
      lst.pop_front();
    }
    return std::move(d);
  }
};

struct _async_read_;

template<typename TgAsyncReadMore>
struct async_read
{
  template<typename T>
  void operator()(T& t,  ::wfc::io::outgoing_handler_t handler)
  {
    if ( !t.status() )
    {
      handler(nullptr);
      return;
    }

    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    auto& clb = t.get_aspect().template get<_callback_list_>();
    
    if ( !lst.empty() && clb.empty() )
    {
      auto d = std::move( lst.front() );
      lst.pop_front();
      handler( std::move(d) );
    }
    else
    {
      clb.push_back(handler);
      t.get_aspect().template get< TgAsyncReadMore >()(t);
    }
  }
};

/*
template<typename Descriptor,  typename TgOutgoing = _incoming_ >
struct stream: fas::aspect<
  fas::type<  ::wfc::io::_descriptor_type_, Descriptor>,
  fas::value<  _outgoing_list_, data_list>,
  fas::value<  _callback_list_, callback_list>,
  
  fas::alias< _output_, _read_incoming_>,
  fas::advice< _read_incoming_, read_incoming<_read_more_> >,
  
  // sync read
  fas::advice< _sync_read_some_,  read_some<_read_handler_> >,
  fas::advice< _sync_read_more_,  read_more<_sync_read_some_> >,
  // methods
  fas::advice< _read_, read<_sync_read_more_> >,
  fas::advice< _async_read_, async_read<_read_more_> >,
  fas::advice<  ::wfc::io::_options_type_, options >,
  aspect< async_read_some, TgOutgoing,  _read_incoming_>
>
{};
*/

}}}
