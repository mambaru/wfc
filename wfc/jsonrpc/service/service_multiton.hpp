//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

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
  
  virtual std::string help(const std::string& args) const override
  {
    std::stringstream ss;
    std::list<std::string> method_list = this->get_method_list();
    if ( args.empty() )
    {
      ss << "Allowed methods for '" << this->name() << "':" << std::endl;
      for (auto n : method_list)
      {
        ss << "  " << n << std::endl;
      }
      
      if ( !method_list.empty() )
      {
        ss << "Get the JSON-RPC schema:"<< std::endl;
        ss << "\t--help " << this->name() << ":schema"    << std::endl;
        ss << "\t--help " << this->name() << ":schema:"   << method_list.front() << std::endl;
        ss << "\t--help " << this->name() << ":params:"   << method_list.front() << std::endl;
        ss << "\t--help " << this->name() << ":result:"   << method_list.front() << std::endl;
        ss << "\t--help " << this->name() << ":request:"  << method_list.front() << std::endl;
        ss << "\t--help " << this->name() << ":response:" << method_list.front() << std::endl;
      }
    }
    else
    {
      typedef typename MethodList::schema_list_t schema_list_t;
      typedef typename schema_list_t::value_type schema_type;
      typedef typename schema_type::json schema_json;
      typedef typename schema_json::list schema_list_json;
        
      size_t schpos = args.find(':');
      std::string schema_name = args.substr(0, schpos);
      std::string method_name = schpos!=std::string::npos 
                             ? args.substr(schpos+1) 
                             : std::string();
        
      schema_list_t schema = MethodList::create_schema();
      if ( schema_name=="schema" && method_name.empty() )
      {
        typename schema_list_json::serializer()(schema, std::ostreambuf_iterator<char>(ss) );
      }
      else
      {
        auto itr = std::find_if(schema.begin(), schema.end(), 
          [method_name](const schema_type& sch)->bool{
            return sch.name == method_name;
          }
        );
        
        if ( itr!=schema.end() )
        {
          using namespace wjson::literals;
          if ( schema_name=="schema" )
            typename schema_json::serializer()(*itr, std::ostreambuf_iterator<char>(ss) );
          else if ( schema_name=="params" )
            ss << itr->params;
          else if ( schema_name=="result" )
            ss << itr->result;
          else if ( schema_name=="request" )
            ss << "{'jsonrpc':'2.0','method':'"_json << method_name << "','params':"_json << itr->params << ",'id':1}"_json;
          else if ( schema_name=="response" )
            ss << "{'jsonrpc':'2.0','result':"_json << itr->result << ",'id':1}"_json;
          else
            ss << "Error: Invalid argument '" << schema_name << "'";
        }
        else
          ss << "ERROR: procedure '"<<  method_name<<"' not found";
      }
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
