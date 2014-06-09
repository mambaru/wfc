#pragma once

#include <wfc/jsonrpc/errors.hpp>
#include <wfc/io/types.hpp>
#include <memory>
#include <functional>

namespace wfc{ namespace jsonrpc{

template<typename Resp>
inline auto mem_fun_make_callback( std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) >&& cb)
  -> std::function<void(std::unique_ptr<Resp>) >
{
  if (cb==nullptr)
    return nullptr;
    
  return [cb]( std::unique_ptr<Resp> resp)
  {
    cb( std::move(resp), nullptr);
  };
}

template<typename Resp>
inline void mem_fun_service_unavailable( std::function< void(std::unique_ptr<Resp>, std::unique_ptr< ::wfc::jsonrpc::error>) >&& cb)
{
  if (cb!=nullptr)
  {
    cb( nullptr, std::make_unique< ::wfc::jsonrpc::error >( ::wfc::jsonrpc::service_unavailable() ) );
  }
}

}} // wfc


