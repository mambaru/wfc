//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wjson/json.hpp>
#include <wjson/name.hpp>
#include <wjson/strerror.hpp>


namespace wfc{ namespace json{

using namespace wjson;

}}

/*
std::string operator "" _json(const char* str, size_t size)
{
  std::string ss( str, str + size);
  std::replace( ss.begin(), ss.end(), '\'', '\"');
  return ss;
}
*/

/*
int main()
{
  std::cout << "{'method':'plus','params':{'first':2,'second':3},'id':1}"_json << std::endl;
}
*/
