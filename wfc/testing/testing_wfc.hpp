#pragma once

#include <wfc/wfc.hpp>
#include <wjson/error.hpp>
#include <memory>

// TODO: удалить после перенесения структур в спп
#include <wjson/wjson.hpp>


namespace wfc{

class core_package_for_testing;

class testing_wfc
{
  typedef std::shared_ptr<basic_wfc> wfc_ptr;
public:
  typedef basic_wfc::package_list package_list;
  typedef basic_wfc::package_ptr package_ptr;

  virtual ~testing_wfc();

  explicit testing_wfc(const package_list& packages = {});

  bool add_configs(const std::string& json, wjson::json_error* er = nullptr, std::string* err_json = nullptr);

  bool add_configs(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json = nullptr);

  void add_package(package_ptr ppack);

  void add_packages(const package_list& packages);

  bool configure(std::function<void(const std::string&, const std::string&, const wjson::json_error&)> error_handler = nullptr);

  bool initialize();

  bool start();


  bool test_service(
    const std::string& srv_name,
    const std::string& method_name,
    const std::string& params_json,
    std::string* result_json = nullptr,
    wjson::json_error* er = nullptr
  );

  bool test_service(
    const std::string& srv_name,
    const std::string& params_json,
    std::string* result_json = nullptr,
    wjson::json_error* er = nullptr
  );

  int run(int poll_count);

private:

  std::string extract_name(const std::string& json, wjson::json_error* er);

  bool add_config_(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json);

  bool add_config_list_(const std::string& comp, const std::string& json, wjson::json_error* er, std::string* err_json);

private:
  wfc_ptr _wfc;

  typedef std::map<std::string, std::string> com_config_t;
  typedef std::map<std::string, com_config_t> all_config_t;
  all_config_t _configs;
  std::shared_ptr<core_package_for_testing> _core;

  bool _configured = false;
  bool _initialized = false;
  bool _started = false;
  bool _fatal = false;
};

} // wfc

