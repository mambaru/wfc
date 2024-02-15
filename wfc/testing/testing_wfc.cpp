
#include "testing_wfc.hpp"
#include <wfc/testing/core_package_for_testing.hpp>
#include <wfc/wfc_build_info.h>
#include <wfc/jsonrpc/ijsonrpc.hpp>
#include <wfc/module/iinstance.hpp>
#include <wlog/wlog.hpp>
#include <iostream>

namespace wfc{

testing_wfc::testing_wfc(const package_list& packages )
  : _wfc(nullptr)
  , _core(std::make_shared<core_package_for_testing>())
{
  package_list pl;
  pl.push_back(_core);
  std::copy(packages.begin(), packages.end(), std::back_inserter(pl));
  _wfc = std::make_shared<basic_wfc>( make_build_info<wfc_build_info>(), pl );

  wlog::logger_options opt;
  wlog::logger_handlers dlh;

  opt.hide = wlog::hide_flags::time;
  opt.customize.resize( opt.customize.size() + 1 );
  opt.customize.back().names.push_back("FATAL");
  opt.customize.back().stdout.color_map["$all"] = "red";

  dlh.date = [](const wlog::time_point&)->std::string { return "\n";};
  dlh.message = [](const std::string& msg)->std::string
  {
    std::string res = msg;
    if (res.size() > 0)
      res.resize(msg.size()-1);
    return res;
  };
  dlh.after.push_back([this](const wlog::time_point& , const std::string&, const std::string& ident, const std::string&)
  {
    if ( ident=="FATAL" )
    {
      _configured = false;
      _initialized = false;
      _started = false;
      _fatal = true;
    }
  });
  wlog::init(opt, dlh);
}

testing_wfc::~testing_wfc()
{
  _wfc = nullptr;
  _core = nullptr;
  wlog::release();
}


bool testing_wfc::add_configs(const std::string& json, wjson::json_error* er, std::string* err_json)
{
  std::map<std::string, std::string> comp_configs;
  wjson::dict_map< wjson::raw_value<> >::serializer()(comp_configs, json.begin(), json.end(), er);
  if (er!=nullptr && *er)
    return false;

  for (const auto& p: comp_configs)
  {
    if ( !this->add_configs(p.first, p.second, er, err_json) )
    {
      return false;
    }
  }
  return true;
}

bool testing_wfc::add_configs(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json)
{
  return this->add_config_list_(comp, json, er, err_json);
}

void testing_wfc::add_package(package_ptr ppack)
{
  _wfc->add_package(ppack);
}

void testing_wfc::add_packages(const package_list& packages)
{
  _wfc->add_packages(packages);
}

bool testing_wfc::configure(std::function<void(const std::string&, const std::string&, const wjson::json_error&)> error_handler )
{
  if ( _fatal )
    return false;

  all_config_t configs = _configs;
  bool res = true;
  _core->get_global()->registry.for_each<icomponent>("component", [&](const std::string& component_name, std::shared_ptr<icomponent> obj)
  {
    if ( !res )
      return;
    auto itr = configs.find(component_name);
    if ( itr!= configs.end() )
    {
      std::vector<std::string> obj_list;
      for (auto p : itr->second)
        obj_list.push_back(p.second);

      std::string comp_json;
      wjson::vector_of< wjson::raw_value<> >::serializer()(obj_list, std::back_inserter(comp_json) );
      wjson::json_error er;
      obj->configure(comp_json, &er);
      if ( er && error_handler!=nullptr )
      {
        res = false;
        error_handler(component_name, comp_json, er);
      }
      configs.erase(itr);
    }
  });
  _configured = res;
  return res;
}

bool testing_wfc::initialize()
{
  if ( !_configured && !this->configure(nullptr) )
    return false;
  _core->get_global()->registry.for_each<iinstance>("instance", [&](const std::string&, std::shared_ptr<iinstance> inst){
    inst->initialize();
  });
  _initialized = true;
  return true;
}

bool testing_wfc::start()
{
  if ( !_initialized && !this->initialize() )
    return false;
  _core->get_global()->registry.for_each<iinstance>("instance", [&](const std::string&, std::shared_ptr<iinstance> inst){
    inst->start("");
  });
  _started = true;
  return true;
}

bool testing_wfc::test_service(
  const std::string& srv_name,
  const std::string& params_json,
  std::string* result_json,
  wjson::json_error* er
)
{
  using namespace wjson::literals;

  auto pservice = _core->get_global()->registry.get_object<ijsonrpc>("",srv_name);

  if ( pservice == nullptr )
  {
    if ( result_json != nullptr )
      *result_json = "{'jsonrpc': '2.0', 'error':{'code':-1,'message':'Service not found.'}, 'id':null}"_json;
    return false;
  }

  wjrpc::incoming_holder inh(params_json);
  inh.parse(er);
  if ( er!=nullptr && *er)
    return false;

  pservice->perform_incoming( std::move(inh), 1, [&](wjrpc::outgoing_holder h)
  {
    if ( result_json!=nullptr )
    {
      if ( auto d = h.detach() )
        *result_json = std::string( d->begin(), d->end() );
    }
  });
  return true;
}


bool testing_wfc::test_service(
  const std::string& srv_name,
  const std::string& method_name,
  const std::string& params_json,
  std::string* result_json,
  wjson::json_error* er
)
{
  using namespace wjson::literals;

  std::stringstream req;
  req << "{'method':'"_json << method_name << "', 'params':"_json << params_json << ", 'id':1}";

  std::string res;

  this->test_service(srv_name, req.str(), &res, er);

  if ( er!=nullptr && *er )
    return false;

  std::map<std::string, std::string> msg_dict;
  wjson::dict_map< wjson::raw_value<> >::serializer()(msg_dict, res.begin(), res.end(), er);

  if ( er!=nullptr && *er )
    return false;

  if ( msg_dict.count("result") == 1 )
  {
    *result_json = msg_dict.at("result");
  }
  else if ( msg_dict.count("error") == 1 )
  {
    *result_json = msg_dict.at("error");
  }
  else
  {
    *result_json = "unknown_error";
    return false;
  }

  return true;
}

int testing_wfc::run(int poll_count)
{
  if ( _fatal )
    return -1;

  if ( !_started && !this->start() )
    return -2;

  _core->set_poll_count(poll_count);
  const char* pname = "test";
  char* argv[1];
  argv[0]=const_cast<char*>(pname);

  return _wfc->run(1, argv, "");
}

// private:

namespace
{
  struct name_config
  {
    std::string name;
  };

  struct name_config_json
  {
    JSON_NAME(name)

    typedef wjson::object<
      name_config,
      wjson::member_list<
        wjson::member<n_name, name_config, std::string, &name_config::name>
      >
    > type;
    typedef type::serializer serializer;
  };
}


std::string testing_wfc::extract_name(const std::string& json, wjson::json_error* er)
{
  name_config name;
  name_config_json::serializer()(name, json.begin(), json.end(), er);
  return name.name;
}

bool testing_wfc::add_config_(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json)
{
  std::string name = extract_name(json, er);
  if ( er!=nullptr && *er )
  {
    if ( err_json!=nullptr )
      *err_json = json;
    return false;
  }

  if ( _core->get_global()->registry.get_object<icomponent>( "component", comp) == nullptr )
  {
    if ( err_json != nullptr)
      *err_json = "Component not found";
    return false;
  }

  _configs[comp][name] = json;
  return true;
}

bool testing_wfc::add_config_list_(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json)
{
  if ( wjson::parser::is_object(json.begin(), json.end()) )
    return add_config_(comp, json, er, err_json);

  std::vector<std::string> configs_list;
  wjson::vector_of< wjson::raw_value<> >::serializer()(configs_list, json.begin(), json.end(), er);
  if ( er!=nullptr && *er )
  {
    if ( err_json != nullptr)
      *err_json = json;
    return false;
  }

  for (const std::string& cur_json: configs_list)
  {
    if ( !add_config_(comp, cur_json, er, err_json) )
      return false;
  }
  return true;
}

} // wfc

