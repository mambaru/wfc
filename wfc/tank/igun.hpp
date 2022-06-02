#pragma once

#include <wfc/iinterface.hpp>
#include <functional>
#include <memory>

namespace wfc
{

struct ibang: public iinterface
{
  typedef std::function<void(bool, bool)> bang_handler;
  virtual bool bang(bang_handler) = 0;
};

template<typename I, typename Opt>
struct igun: public ibang
{
  typedef I interface_type;
  typedef std::shared_ptr<interface_type> interface_ptr;
  typedef Opt options_type;
  typedef typename options_type::request_type request_type;
  typedef typename options_type::response_type response_type;
  typedef typename options_type::user_options user_options;
  virtual void initialize( interface_ptr target, const options_type&) = 0;
  virtual void stop() = 0;

};

}
