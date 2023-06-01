#include "vars.hpp"
#include "detail/private_vars.hpp"
#include <wjson/wjson.hpp>

#include <boost/regex.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace wfc{

vars::vars(find_file_f fff)
  : find_file_(fff)
{
  if ( find_file_ == nullptr)
    find_file_ = [](const std::string& file) noexcept -> std::string  { return file;};
}

bool vars::status() const
{
  return _error_code == error_codes::SUCCESS;
}

void vars::reset_status()
{
  _error_code=error_codes::SUCCESS;
  _error_message.clear();
}

vars::error_codes vars::error_code() const
{
  return _error_code;
}

std::string vars::error_message() const
{
  return _error_message;
}

const vars::ini_map_t& vars::ini_map() const
{
  return _ini_map;
}

const vars::include_map_t& vars::include_map() const
{
  return _include_map;
}

const std::string& vars::result() const
{
  return _result;
}

bool vars::add_ini(const std::vector<std::string>& ini_files)
{
  bool ini_status = true;
  for (const std::string& ini : ini_files)
  {
    // Не проверяем статус, пытаемся прочесть остальные
    ini_status &= this->read_ini(ini);
  }
  return ini_status;
}

bool vars::read_ini( const std::string& file)
{
  std::string fullname = find_file_(file);
  if ( fullname.empty() )
    return false;
  std::string ini_text;
  if ( !this->parse_file(fullname, &ini_text) )
    return false;

  _include_map[fullname]++;

  return this->parse_ini(ini_text);
}

bool vars::parse_ini( const std::string& text)
{
  std::istringstream is(text);
  boost::property_tree::ptree ptree;
  boost::property_tree::ini_parser::read_ini(is, ptree);

  bool ini_status = true;
  for (auto& section : ptree)
  {
    if ( section.second.size() )
    {
      for (auto& key : section.second)
      {
        auto& curval = _ini_map[section.first + "."+ key.first];
        auto newval = key.second.get_value<std::string>();
        std::string err;
        if ( !detail::vars::parse_raw_string( newval, &curval, &err) )
          ini_status = set_error_(error_codes::BAD_STRING, newval + ": " + err);
      }
    }
    else
    {
      auto& curval = _ini_map[section.first];
      auto newval = section.second.get_value<std::string>();
      std::string err;
      if ( !detail::vars::parse_raw_string( newval, &curval, &err) )
        ini_status = set_error_(error_codes::BAD_STRING, newval + ": " + err);
    }
  }
  return ini_status;
}

bool vars::parse_file(const std::string& file) noexcept
{
  return this->parse_file(file, &_result);
}

bool vars::parse_file( const std::string& file, std::string* text) noexcept
try
{
  if ( file.empty() )
  {
    _error_code = error_codes::BAD_FILE;
    _error_message = "Empty file name(vars::parse_file)";
    return false;
  }

  auto filename = find_file_(file);

  std::ifstream ifs(filename);
  if ( !ifs )
  {
    _error_code = error_codes::BAD_FILE;
    _error_message = "Bad file: '" + filename + "'";
    return false;
  }


  std::string in_text;
  std::copy(
    std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(),
    std::back_inserter(in_text));

  return this->parse_text(in_text, text);
}
catch(const std::exception& e)
{
  _error_code = error_codes::EXCEPTION;
  _error_message = "Bad file '" + file + "': " + e.what();
  return false;
}

bool vars::parse_text(const std::string& intxt, std::string* text) noexcept
try
{
  if ( text!=nullptr )
    *text = std::move(intxt);

  std::string err;
  bool status = true;
  status &= this->apply_VAR(text);
  status &= this->apply_ENV(text);
  status &= this->apply_INI(text);
  status &= this->apply_INCLUDE( text );
  return status;
}
catch(const std::exception& e)
{
  _error_code = error_codes::EXCEPTION;
  _error_message = e.what();
  return false;
}

bool vars::parse_text(const std::string& intxt) noexcept
{
  return this->parse_text(intxt, &_result);
}


bool vars::apply(const std::string& prefix, const apply_handler_f& handler)
{
  return this->apply(prefix, handler, &_result);
}

bool vars::apply(const std::string& prefix, const apply_handler_f& handler, std::string* text)
try
{
  vars_list_t vars_list;

  if ( !this->search_vars(*text, &vars_list) )
    return false;

  if ( vars_list.empty() )
    return true;

  replace_list_t replace_list;

  for (const var_info& vi: vars_list)
  {
    if ( vi.prefix==prefix )
    {
      replace_list.push_back({vi.prefix,vi.name,vi.defval});
      replace_list.back().newval = handler(vi);
    }
  }

  if ( replace_list.empty() )
    return true;

  return this->replace_vars( replace_list, text, nullptr);
}
catch(const std::exception& e)
{
  _error_code = error_codes::EXCEPTION;
  _error_message = e.what();
  return false;
}

bool vars::apply_INCLUDE(std::string* text)
{
  apply_handler_f handler = [this](const var_info& vi)
  {
    ini_map_t cur_var;
    cur_var=_var_map;
    std::string newval = vi.defval;
    std::string path;
    for (auto opt: vi.options)
    {
      if ( opt.first == "path" )
        path=opt.second;
      else
        _var_map[opt.first] = opt.second;
    }

    if ( !path.empty() )
    {
      if ( vi.name=="FILE" || vi.name=="SUBCONF"  )
      {
        std::string fullname = find_file_(path);
        if ( this->parse_file(fullname, &newval) )
          _include_map[fullname]++;
      }

      if ( vi.name=="SUBCONF" )
      {
        wjson::json_error je;
        auto beg = wjson::parser::parse_space(newval.begin(), newval.end(), &je);
        if ( !je )
        {
          auto end = wjson::parser::parse_object(beg, newval.end(), &je);
          if ( !je )
          {
            beg = wjson::parser::parse_space(beg+1, end, &je);
            end = wjson::parser::parse_space(
              std::reverse_iterator<std::string::iterator>(end)+1,
              std::reverse_iterator<std::string::iterator>(beg),
              &je
            ).base();
            newval.assign(beg, end);
          }
        }

        if ( je )
        {
          std::cerr << wjson::strerror::message_trace(je, newval.begin(), newval.end() ) << std::endl;
          newval.clear();
        }
      }
    }
    cur_var.swap(_var_map);
    return newval;
  };

  return vars::apply("INCLUDE", handler, text);
}

bool vars::apply_ENV( std::string* text)
{
  apply_handler_f handler = [](const var_info& vi)
  {
    std::string newval = vi.defval;
    vars::get_env(vi.name, &newval);
    return newval;
  };

  return vars::apply("ENV", handler, text);
}

bool vars::apply_INI( std::string* text)
{
  apply_handler_f handler = [this](const var_info& vi)
  {
    std::string newval = vi.defval;
    auto itr = _ini_map.find(vi.name);
    if ( itr !=  _ini_map.end() )
      newval = itr->second;
    return newval;
  };

  return vars::apply("INI", handler, text);
}

bool vars::apply_VAR( std::string* text)
{
  apply_handler_f handler = [this](const var_info& vi)
  {
    std::string newval = vi.defval;
    auto itr = _var_map.find(vi.name);
    if ( itr !=  _var_map.end() )
      newval = itr->second;
    return newval;
  };

  return vars::apply("VAR", handler, text);
}
bool vars::search_vars( vars_list_t* vars_list ) const noexcept
{
  return this->search_vars(_result, vars_list);
}


bool vars::search_vars( const std::string& text, vars_list_t* vars_list) const noexcept
{
  std::vector<std::string> raw_vars_list;
  raw_vars_list.reserve(16);
  if (!this->search_vars(text, &raw_vars_list))
    return false;
  for ( const std::string& raw_var : raw_vars_list)
  {
    var_info var;
    if ( !vars::match_var(raw_var, &var, nullptr) )
      return false;
    if ( vars_list!=nullptr )
      vars_list->push_back(var);
  }
  return true;
}

bool vars::search_vars( const std::string& text, std::vector<std::string>* vars_list) const noexcept
try
{
  auto beg = text.begin();
  auto end = text.end();
  boost::match_results<std::string::const_iterator> results;
  boost::regex reg("\\$[\\w]*?\\{[^$]+?\\}");
  boost::match_flag_type flags = boost::match_default;

  while ( boost::regex_search(beg, end, results,  reg, flags ) )
  {
    for ( auto& res: results )
    {
      if ( vars_list!=nullptr)
      {
        vars_list->push_back(res.str());
      }
    }
    beg = results[0].second;
    flags |= boost::match_prev_avail;
    flags |= boost::match_not_bob;
  }
  return true;
}
catch(const boost::regex_error& be)
{
  _error_code = error_codes::EXCEPTION;
  _error_message = be.what();
  return false;
}
catch(const std::exception& e)
{
  _error_code = error_codes::EXCEPTION;
  _error_message = e.what();
  return false;
}

bool vars::replace_vars(const replace_list_t& replace_list, replace_list_t* not_found)
{
  return this->replace_vars(replace_list, &_result, not_found);
}

bool vars::replace_vars(const replace_list_t& replace_list, std::string* text, replace_list_t* not_found)
{
  if ( text == nullptr )
    return true;

  std::stringstream sout;
  std::string::const_iterator beg = text->cbegin();
  std::string::const_iterator end = text->cend();
  replace_list_t cur_list =  replace_list;

  boost::regex any_reg("\\$[\\w]*?\\{[^$]+?\\}");
  boost::match_results<std::string::const_iterator> results;
  while ( boost::regex_search(beg, end, results,  any_reg ) )
  {
    auto itr = cur_list.begin();
    for (; itr!=cur_list.end(); ++itr)
    {
      std::string regstr = "\\$"+itr->prefix+"{"+itr->name + "((\\})|([ :[][^}]*\\}))";
      boost::regex reg(regstr);
      boost::match_results<std::string::const_iterator> results2;
      if ( boost::regex_match(results[0].first, results[0].second, results2, reg ) )
        break;
    }

    sout << std::string(beg, results[0].first );
    if ( itr != cur_list.end() )
    {
      sout << itr->newval;
      cur_list.erase(itr);
    }
    else
      sout << std::string(results[0].first, results[0].second );
    beg = results[0].second;
  }

  if (beg!=end)
    sout << std::string(beg, end);

  *text = sout.str();
  if ( not_found != nullptr)
    not_found->swap(cur_list);

  return true;
}

bool vars::set_env(const std::string& name, const std::string& val, bool overwrite)
{
  return ::setenv( name.c_str(), val.c_str(), static_cast<int>(overwrite) ) == 0;
}

bool vars::get_env(const std::string& name, std::string* val)
{
  const char * env = std::getenv( name.c_str() );
  if  ( env!=nullptr && val!=nullptr)
    *val = env;
  return env!=nullptr;
}

std::string vars::get_env(const std::string& name)
{
  std::string val;
  get_env(name, &val);
  return val;
}

bool vars::match_var( const std::string& text, var_info* var, std::string* err)
try
{
  boost::match_results<std::string::const_iterator> results;
  boost::regex reg("\\$(\\w*)?\\{([\\w.-]+)?(\\[.*\\])?\\s*:?\\s*(.*)?\\}");
  if ( !boost::regex_match(text, results,  reg ) )
  {
    if ( err != nullptr )
      *err = "Regex match error:" + text;
    return false;
  }

  if ( var!=nullptr )
  {
    var->prefix = results[1].str();
    var->name = results[2].str();

    std::string raw_params = results[3].str();
    if ( !raw_params.empty() )
    {
      raw_params.erase(raw_params.begin());
      raw_params.erase( (raw_params.rbegin()+1).base() );
      if ( !detail::vars::parse_option_list(raw_params, &var->options, err) )
        return false;
    }

    std::string raw_defval = results[4].str();
    if ( !raw_defval.empty() )
    {
      if ( !detail::vars::parse_value(raw_defval, &var->defval, err) )
        return false;
    }
  }
  return true;
}
catch(const boost::regex_error& be)
{
  if ( err!=nullptr )
    *err = be.what();
  return false;
}
catch(const std::exception& e)
{
  if ( err!=nullptr )
    *err = e.what();
  return false;
}

bool vars::set_error_(error_codes ec, const std::string& msg)
{
  _error_code = ec;
  switch( ec )
  {
    case error_codes::SUCCESS: _error_message.clear() ; break;
    case error_codes::BAD_FILE: _error_message = "Bad file: " + msg ; break;
    case error_codes::INCLUDE_LIMIT: _error_message = "File inclusion limit exceeded for: " + msg ; break;
    case error_codes::BAD_STRING: _error_message = "Bad string: " + msg ; break;
    case error_codes::EXCEPTION: _error_message = "Exception: " + msg ; break;
    default: break;
  }
  return this->status();
}

}
