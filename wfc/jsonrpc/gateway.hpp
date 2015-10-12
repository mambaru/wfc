#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <iow/jsonrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename JsonrpcHandler>
class gateway_impl
  : public basic_domain<Interface, JsonrpcHandler>
{
  typedef basic_domain<Interface, JsonrpcHandler> super;
public:
  typedef typename super::engine_options engine_options;
  typedef typename super::interface_type interface_type;

  virtual void reconfigure() override
  {
    auto dopt = this->options();
    engine_options eopt = static_cast<engine_options>(dopt);
    
    

    super::reconfigure_(eopt);
    
    /*
    std::cout << "--- engine reconfigure --- -0-" << std::endl;
    const auto& domain_opt = this->options();
    typedef typename engine_type::target_type target_type;
    typedef typename target_type::element_type interface_type;
    target_type target = this->global()->registry.template get< interface_type >(domain_opt.target);
    engine_options engine_opt = static_cast<engine_options>(domain_opt);
    engine_opt.target = target;
    engine_opt.peeper = target;
    */
  }
  
};

template<typename MethodList, template<typename> class ItfT = interface_implementation>
class gateway
  : public ItfT< gateway_impl< typename MethodList::interface_type, ::iow::jsonrpc::handler< ItfT<MethodList> > > >

{
public:
  virtual ~gateway()
  {
    std::cout << "~gateway() " << std::endl;
  }

};

}}