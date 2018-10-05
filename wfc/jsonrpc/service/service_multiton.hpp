#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/service/service.hpp>
#include <wfc/jsonrpc/service/service_options_json.hpp>
#include <wfc/jsonrpc/statistics_options_json.hpp>
#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>

namespace wfc{ namespace jsonrpc{

/**
 * @brief service_multiton
 * @tparam Name
 * @tparam MethodList
 * @tparam Impl
 **/
template< typename Name, typename MethodList, template<typename> class Impl = interface_implementation >
class service_multiton: public ::wfc::multiton<
  Name,
  ::wfc::instance< service< MethodList, Impl > >,
  ::wfc::jsonrpc::service_options_json< typename service< MethodList, Impl >::options_type >,
  ::wfc::component_features::DisabledPriority,
  statistics_options_json
>
{
public:
  virtual std::string interface_name() const override
  {
    return "ijsonrpc";
  }
  
  std::list<std::string> get_method_list() const
  {
    std::list<std::string> result;
    typedef typename MethodList::aspect::template select_group< wjrpc::_method_ >::type tag_list;
    this->get_method_list_(result, tag_list() );
    return result;
  }
  
  virtual std::string help() const override
  {
    std::stringstream ss;
    ss << "Allowed methods for '" << this->name() << "':" << std::endl;
    std::list<std::string> method_list = this->get_method_list();
    for (auto n : method_list)
    {
      ss << "  " << n << std::endl;
    }
    return ss.str();
  }

private:
  
  static void get_method_list_(std::list<std::string>& , fas::empty_list) { }
 
  template<typename L>
  void get_method_list_(std::list<std::string>& res, L) const
  {
    typedef typename fas::head<L>::type tag;
    res.push_back( tag()() );
    this->get_method_list_( res, typename fas::tail<L>::type() );
  }

  
};

}}
