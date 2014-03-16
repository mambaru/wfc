#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/callback/callback_status.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{
  
template<
  typename Interface,
  typename Target, 
  void (Target::*mem_ptr)( size_t, std::weak_ptr<Interface> ) 
>
struct mem_fun_startup
{
  template<typename T>
  void operator()(T& t, ::wfc::io::io_id_t id) const
  {
    if (auto trg = t.target().lock() )
    {
      (trg.get()->*mem_ptr)( id, t.shared_from_this() );
      
    }
  }
};

template<
  typename Target, 
  void (Target::*mem_ptr)( size_t ) 
>
struct mem_fun_shutdown
{
  template<typename T>
  void operator()(T& t, ::wfc::io::io_id_t id) const
  {
    if (auto trg = t.target().lock() )
    {
      (trg.get()->*mem_ptr)( id );
      
    }
  }
};
  
template<
  typename Req, 
  typename Resp, 
  typename I, 
  void (I::*mem_ptr)( std::unique_ptr<Req>, std::function< void(std::unique_ptr<Resp>) > ) 
>
struct mem_fun_handler
{
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
  {
    if ( auto i = t.target().lock() )
    {
      if ( callback==nullptr)
      {
        (i.get()->*mem_ptr)( std::move(req), nullptr );
      }
      else
      {
        (i.get()->*mem_ptr)( std::move(req), [callback](std::unique_ptr<Resp> resp)
        {
          callback( std::move(resp), nullptr);
        });
      }
    }
  }
};

template<
  typename Req, 
  typename Resp, 
  typename Target, 
  typename Itf, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<Req>, 
    std::function< void(std::unique_ptr<Resp>) >, 
    size_t, 
    std::weak_ptr<Itf>
  ) 
>
struct mem_fun_handler_ex
{
  
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
  {
    if ( auto i = t.target().lock() )
    {
      if ( callback==nullptr)
      {
        (i.get()->*mem_ptr)( std::move(req), nullptr, t.get_id(), t.shared_from_this() );
      }
      else
      {
        (i.get()->*mem_ptr)( std::move(req), [callback](std::unique_ptr<Resp> resp)
        {
          callback( std::move(resp), nullptr);
        }, t.get_id(), t.shared_from_this() );
      }
    }
  }
};


}} // wfc


