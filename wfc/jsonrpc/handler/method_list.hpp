#pragma once

#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/handler_aspect.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template<typename... Args >
class method_list_base
  : public fas::aspect_class< typename fas::merge_aspect< fas::aspect<Args...>, handler_aspect>::type >
{
public:
  typedef fas::aspect_class< typename fas::merge_aspect< fas::aspect<Args...>, handler_aspect>::type > super;
  typedef typename super::aspect::template advice_cast<_target_>::type target_type;
  typedef typename super::aspect::template advice_cast<_interface_>::type interface_type;
};


template<typename... Args >
class method_list
  : public handler_base
  , public method_list_base<Args...>
  , public method_list_base<Args...>::interface_type
{
public:
  typedef method_list<Args...> self;
  typedef self magic;
  typedef method_list_base<Args...> super;
  typedef typename super::target_type    target_type;
  typedef typename super::interface_type interface_type;

  template<typename PReq, typename Serializer>
  void send_request( const char* name, PReq req, Serializer ser, incoming_handler_t  clb) const
  {
    
    std::shared_ptr<PReq> p = nullptr;
    if (req!=nullptr)
      p = std::make_shared<PReq>( std::move(req) );
    outgoing_request_handler(
      name,
      std::move(clb), // обработчик ответ
      [p, ser](const char* name, int id)-> ::wfc::io::data_ptr
      {
        
        return ser(name, std::move(*p), id);
      }
    );
  }
  
  
  template<typename Tg>
  struct call_params_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type::aspect::template advice_cast<_call_>::type::request_ptr type;
  };

  template<typename Tg>
  struct call_result_ptr
  {
    typedef typename super::aspect::template advice_cast<Tg>::type::aspect::template advice_cast<_call_>::type::response_ptr type;
  };
  
  template<typename Tg>
  struct call_error_ptr
  {
    typedef std::unique_ptr< ::wfc::jsonrpc::error> type;
  };

  template<typename Tg, typename ReqPtr, typename Callback>
  void call(ReqPtr req, Callback callback) const
  {
    
    this->get_aspect().template get<Tg>().call( *this, std::move(req), callback);
  }

  
  
};


}} // wfc


