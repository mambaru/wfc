#include "private_vars.hpp"
#include <wjson/wjson.hpp>
#include <string>

namespace wfc{ namespace detail{

bool vars::parse_option_list( const std::string& text, option_list_t* options, std::string* err)
{
  vars::parse_option_list(text.begin(), text.end(), options, err);
  return err==nullptr || err->empty();
}

vars::const_iterator vars::parse_option_list( const_iterator beg, const_iterator end,
                                              option_list_t* options, std::string* err)
{
  while (beg!=end)
  {
    std::string name, value;
    beg = vars::parse_option(beg, end, &name, &value, err);
    if ( err!=nullptr && !err->empty() )
      return beg;
    if ( options!=nullptr )
        options->push_back( option_type(name, value) );
    if (beg!=end && *beg==',' )
      ++beg;
  }
  return beg;
}


bool vars::parse_option( const std::string& text, std::string* name, std::string* val, std::string* err)
{
  vars::parse_option(text.begin(), text.end(), name, val, err);
  return err==nullptr || err->empty();
}

vars::const_iterator vars::parse_option( const_iterator beg, const_iterator end, std::string* name, std::string* val, std::string* err)
{
  beg = vars::parse_value(beg, end, name, err);
  if ( err!=nullptr && !err->empty() )
    return beg;

  if ( beg == end || *beg != '=' )
  {
    if ( name!=nullptr && val!=nullptr)
      val->swap(*name);
    return beg;
  }
  return vars::parse_value(++beg, end, val, err);
}


bool vars::parse_value( const std::string& text, std::string* val, std::string* err)
{
  vars::parse_value(text.begin(), text.end(), val, err);
  return err==nullptr || err->empty();
}

vars::const_iterator vars::parse_value( const_iterator beg, const_iterator end, std::string* val, std::string* err)
{
  wjson::json_error je;
  beg = wjson::parser::parse_space(beg, end, &je);
  if ( !je )
  {
    if (wjson::parser::is_string(beg, end) )
    {
      if ( val != nullptr )
        beg = wjson::value<std::string>::serializer()(*val, beg, end, &je);
      else
        beg = wjson::parser::parse_string(beg, end, &je);
    }
    else
    {
      if ( val != nullptr )
        for (;beg!=end && *beg > ' ' && *beg!=',' && *beg!='='; ++beg )
          val->push_back(*beg);
    }
    if (!je)
      beg = wjson::parser::parse_space(beg, end, &je);
  }
  if ( je && err!=nullptr )
  {
    *err = wjson::strerror::message(je);
  }
  return beg;
}


bool vars::parse_raw_string( const std::string& text, std::string* val, std::string* err )
{
  wjson::json_error je;
  if (wjson::parser::is_string(text.begin(), text.end() ) )
  {
    if ( val != nullptr )
      wjson::value<std::string>::serializer()(*val, text.begin(), text.end(), &je);
    else
      wjson::parser::parse_string(text.begin(), text.end(), &je);
  }
  else if ( val != nullptr )
    *val = text;

  if ( je && err!=nullptr )
    *err = wjson::strerror::message(je);
  return !je;
}

}}
