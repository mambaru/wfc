//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2022
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <sstream>

namespace wfc{

/**
 * @brief Класс для подстановки в конфигурационные и прочие файлы переменных из среды окружения, ini-файлов и
 * пользовтельских данных с поддержкой включения (INCLUDE) внешних файлов любой глубины вложенности
 *
 * Общий формат переменных $*PREFIX*{*VAR*[opt1=val1, opt2="val2", val3]:defval}. В *PREFIX* и *VAR* допускаются только буквенно-
 * цифровые символы, дополнительно для **VAR** допустимы точка и минус, пробелы между ${[ не допускаются. В квадратных скобках список опций, определяется индивидуально для каждой переменной,
 * значение опции можно взять в кавычки, тогда оно рассматривается как json-срока и десериализуется. Также можно взять в кавычки значение defval.
 *
 * Следующие переменные обрабатываются при загрузке файла:
 *    - $**ENV**{VAR:defval} - подстановка переменных окружения
 *    - $**INI**{sections.VAR:defval} - подстановка переменных из предварительно загруженных ini-файлов
 *    - $**INCLUDE**{**FILE**[**path**="/path/to/file.txt"]} - подстановка содержимого файла с предварительной заменой в нем описанных здесь переменных,
 *      включая INCLUDE.
 *
 * @code
 *    wfc::vars _vars;
 *    _vars.add_ini({"ini1.ini","ini2.ini"});
 *    if ( _vars.parse_file("example.conf") )
 *      std::cout << _vars.result() << endl;
 *    else
 *      std::cout << _vars.error_message() << std::endl;
 * @endcode
 *
 */
class vars
{
private:
  typedef std::map<std::string, size_t> include_map_t;

public:
  enum class error_codes
  {
    SUCCESS,
    BAD_FILE,
    INCLUDE_LIMIT,
    EXCEPTION,
    BAD_STRING,
  };

  typedef std::pair<std::string, std::string> option_type;
  typedef std::vector<option_type> option_list_t;
  struct var_info
  {
    option_list_t options;
    std::string prefix;
    std::string name;
    std::string defval;
  };
  typedef std::vector<var_info> vars_list_t;

  struct replace_value
  {
    std::string prefix;
    std::string name;
    std::string newval;
  };
  typedef std::vector<replace_value> replace_list_t;

  typedef std::map<std::string,std::string> ini_map_t;
  typedef std::function<std::string(const var_info&)> apply_handler_f;
public:

  bool add_ini(const std::vector<std::string>& ini_files);
  bool read_ini( const std::string& file);
  bool parse_ini( const std::string& text);

  bool parse_file(const std::string& file) noexcept;
  bool parse_file(const std::string& file, std::string* text) noexcept;
  bool parse_text(const std::string& intxt, std::string* text) noexcept;
  bool parse_text(const std::string& intxt) noexcept;

  bool search_vars( vars_list_t* vars_list ) const noexcept;
  bool search_vars( const std::string& text, vars_list_t* vars_list) const noexcept;
  bool search_vars( const std::string& text, std::vector<std::string>* vars_list) const noexcept;

  bool replace_vars( const replace_list_t& replace_list, replace_list_t* not_found = nullptr);
  bool replace_vars( const replace_list_t& replace_list, std::string* text, replace_list_t* not_found = nullptr);

  bool apply(const std::string& prefix, const apply_handler_f& handler);
  bool apply(const std::string& prefix, const apply_handler_f& handler, std::string* text);
  bool apply_INCLUDE( std::string* text);
  bool apply_ENV( std::string* text);
  bool apply_INI( std::string* text);

  void reset_status();
  bool status() const;
  error_codes error_code() const;
  std::string error_message() const;
  const std::string& result() const;
  const ini_map_t& ini_map() const;

  static bool set_env(const std::string& name, const std::string& val, bool overwrite = false);
  static bool get_env(const std::string& name, std::string* val);
  static std::string get_env(const std::string& name);
  static bool match_var( const std::string& text, var_info* var, std::string* err);

  template<typename T>
  static T get_opt(const var_info& v, const std::string& name, T defval = T())
  {
    auto itr = std::find_if(v.options.begin(), v.options.end(), [&name](const option_type& p){
      return p.first == name;
    });
    if ( itr == v.options.end() )
      return defval;

    std::istringstream ss(itr->second);

    T res = T();
    ss >> res;

    return res;
  }
private:
  bool set_error_(error_codes ec, const std::string& msg = "");
private:
  mutable error_codes _error_code = error_codes::SUCCESS;
  mutable std::string _error_message;
  include_map_t _include_map;
  ini_map_t _ini_map;
  std::string _result;
};

}

