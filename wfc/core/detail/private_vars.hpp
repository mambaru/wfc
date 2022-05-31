#pragma once

#include <string>
#include <vector>
#include <utility>

namespace wfc{ namespace detail{

class vars
{
public:
  typedef std::pair<std::string, std::string> option_type;
  typedef std::vector<option_type> option_list_t;
  typedef std::string::iterator iterator;
  typedef std::string::const_iterator const_iterator;


  static bool parse_option_list( const std::string& text, option_list_t* options, std::string* err);
  static const_iterator parse_option_list( const_iterator beg, const_iterator end, option_list_t* options, std::string* err);

  static bool parse_option( const std::string& text, std::string* name, std::string* val, std::string* err);
  static const_iterator parse_option( const_iterator beg, const_iterator end, std::string* name, std::string* val, std::string* err);

  static bool parse_value( const std::string& text, std::string* val, std::string* err);
  static const_iterator parse_value( const_iterator beg, const_iterator end, std::string* val, std::string* err);

  static bool parse_raw_string( const std::string& text, std::string* val, std::string* err );

};

}}

