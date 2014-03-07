#pragma once

#include <wfc/jsonrpc/handler/handler_base.hpp>
#include <wfc/jsonrpc/handler/handler_aspect.hpp>
#include <wfc/jsonrpc/handler/tags.hpp>
#include <fas/aop.hpp>

namespace wfc{ namespace jsonrpc{

template<typename... Args >
class method_list
  : public handler_base
  , public fas::aspect_class< typename fas::merge_aspect< fas::aspect<Args...>, handler_aspect>::type >
  , public std::enable_shared_from_this< method_list<Args...> >
{
public:
  typedef method_list<Args...> self;
  typedef self magic;
  typedef fas::aspect_class< typename fas::merge_aspect< fas::aspect<Args...>, handler_aspect>::type > super;
  typedef typename super::aspect::template advice_cast<_target_>::type target_type;
  
  template<typename PReq, typename Serializer, typename Callback>
  void send_request( const char* , PReq , Serializer , Callback  )
  {
    
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
    typedef std::unique_ptr<wfc::jsonrpc::error> type;
  };

  template<typename Tg, typename ReqPtr, typename Callback>
  void call(ReqPtr req, Callback callback)
  {
    this->get_aspect().template get<Tg>().call( *this, std::move(req), callback);
  }

  
  
  /*
  template<typename Tg>
  void request(
    typename super::aspect::template advice_cast<Tg>::type::aspect::template advice_cast<_call_>::type::request_ptr req,
    std::function< void (
      typename super::aspect::template advice_cast<Tg>::type::aspect::template advice_cast<_call_>::type::response_ptr, 
      typename super::aspect::template advice_cast<Tg>::type::aspect::template advice_cast<_call_>::type::error_ptr
    )> callback
  )
  {
  }
  */
};


}} // wfc


