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
    if (auto trg = t.provider().lock() )
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
    if (auto trg = t.provider().lock() )
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
    else if (callback!=nullptr)
    {
      callback( nullptr, std::make_unique< ::wfc::jsonrpc::error >( ::wfc::jsonrpc::service_unavailable() ) );
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
    else if (callback!=nullptr)
    {
      callback( nullptr, std::make_unique< ::wfc::jsonrpc::error >( ::wfc::jsonrpc::service_unavailable() ) );
    }
  }
};


template<
  typename Req, 
  typename Resp, 
  typename Req2, 
  typename Resp2, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<Req>, 
    std::function< void(std::unique_ptr<Resp>) >, 
    size_t, 
    std::function< void(
      std::unique_ptr<Req2>, 
      std::function< void(std::unique_ptr<Resp2>) >  
    ) >
  ),
  typename Itf,
  void (Itf::*mem_ptr2)( 
    std::unique_ptr<Req2>, 
    std::function< void(std::unique_ptr<Resp2>) >
  )
>
struct mem_fun_handler2
{
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
  {
    if ( auto i = t.target().lock() )
    {
      std::weak_ptr<Itf> self = t.shared_from_this();
      (i.get()->*mem_ptr)
      (
        std::move(req),
        this->make_callback(callback),
        t.get_id(),
        [self](std::unique_ptr<Req2> req, std::function< void(std::unique_ptr<Resp2>) > callback)
        {
          if ( auto p = self.lock() )
          {
            (p.get()->*mem_ptr2)(
              std::move(req),
              callback
            );
          }
        }
      );
    }
    else if (callback!=nullptr)
    {
      callback( nullptr, std::make_unique< ::wfc::jsonrpc::error >( ::wfc::jsonrpc::service_unavailable() ) );
    }
  }
  
  
  auto make_callback( std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
    -> std::function<void(std::unique_ptr<Resp>) >
  {
    if (callback==nullptr)
      return nullptr;
    
    return [callback]( std::unique_ptr<Resp> resp)
    {
      callback( std::move(resp), nullptr);
    };
  }
  
};

template<
  typename Req, 
  typename Resp, 
  typename Target, 
  void (Target::*mem_ptr)( 
    std::unique_ptr<Req>, 
    std::function< void(std::unique_ptr<Resp>) >, 
    size_t
  )
>
struct mem_fun_handler3
{
  template<typename T>
  void operator()(T& t, std::unique_ptr<Req> req, std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
  {
    if ( auto i = t.target().lock() )
    {
      
      (i.get()->*mem_ptr)
      (
        std::move(req),
        this->make_callback(callback),
        t.get_id()
      );
    }
    else if (callback!=nullptr)
    {
      callback( nullptr, std::make_unique< ::wfc::jsonrpc::error >( ::wfc::jsonrpc::service_unavailable() ) );
    }
  }
  
  
  auto make_callback( std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) > callback) const
    -> std::function<void(std::unique_ptr<Resp>) >
  {
    if (callback==nullptr)
      return nullptr;
    
    return [callback]( std::unique_ptr<Resp> resp)
    {
      callback( std::move(resp), nullptr);
    };
  }
  
};


}} // wfc


