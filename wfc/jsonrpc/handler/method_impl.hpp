#pragma once

#include <memory>
#include <functional>

// TODO: убрать 
/*
#define JSONRPC_METHOD_IMPL(Tg, Method)\
  virtual void Method( call_params_ptr<Tg>::type req, std::function< void(call_result_ptr<Tg>::type) > callback)\
  {\
    if ( callback == nullptr )\
    {\
      this->call<Tg>( std::move(req), nullptr );\
    }\
    else\
    {\
      this->call<Tg>( std::move(req), [callback](call_result_ptr<Tg>::type resp, call_error_ptr<Tg>::type error)\
      {\
        if ( error==nullptr){\
          if ( resp != nullptr) \
            callback( std::move(resp) );\
          else\
            callback( nullptr );\
        };\
      });\
    }\
  }

#define JSONRPC_METHOD_EMPTY(Tg, Method)\
  virtual void Method( call_params_ptr<Tg>::type , std::function< void(call_result_ptr<Tg>::type) > )\
  {\
  }
  
#define JSONRPC_METHOD_IMPL_EX(Tg, Method)\
  virtual void Method( call_params_ptr<Tg>::type req, std::function< void(call_result_ptr<Tg>::type) > callback, size_t, target_type)\
  {\
    if ( callback == nullptr )\
    {\
      this->call<Tg>( std::move(req), nullptr );\
    }\
    else\
    {\
      this->call<Tg>( std::move(req), [callback](call_result_ptr<Tg>::type resp, call_error_ptr<Tg>::type error)\
      {\
        if ( error==nullptr){\
          if ( resp != nullptr) \
            callback( std::move(resp) );\
          else\
            callback( nullptr );\
        };\
      });\
    }\
  }

// handler - Игнорируеться, т.к. данные будут поступать через Таргет
// TODO: внедрить Таргет
#define JSONRPC_METHOD_IMPL2(Tg, Method, Tg2)\
  virtual void Method( call_params_ptr<Tg>::type req, std::function< void(call_result_ptr<Tg>::type) > callback, size_t, std::function< void(call_params_ptr<Tg2>::type, std::function< void(call_result_ptr<Tg2>::type) >) > )\
  {\
    if ( callback == nullptr )\
    {\
      this->call<Tg>( std::move(req), nullptr );\
    }\
    else\
    {\
      this->call<Tg>( std::move(req), [callback](call_result_ptr<Tg>::type resp, call_error_ptr<Tg>::type error)\
      {\
        if ( error==nullptr){\
          if ( resp != nullptr) \
            callback( std::move(resp) );\
          else\
            callback( nullptr );\
        };\
      });\
    }\
  }

  

#define JSONRPC_METHOD_EMPTY_EX(Tg, Method)\
  virtual void Method( call_params_ptr<Tg>::type , std::function< void(call_result_ptr<Tg>::type, size_t, target_type) > )\
  {\
  }

#define JSONRPC_STARTUP_EMPTY(Tg, Method)\
  virtual void Method(size_t, target_type) > )\
  {\
  }

#define JSONRPC_SHUTDOWN_EMPTY(Tg, Method)\
  virtual void Method(size_t)\
  {\
  }

  */

