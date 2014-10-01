#pragma once

#include <wfc/logger/ilogger.hpp>
#include <sstream>
#include <string>
#include <memory>
#include <iostream>

namespace wfc{

class logstream
{
public:
  ~logstream()
  {
    if (_logger)
      _logger->write(_name, _ident, _ss.str());
    else
    {
#ifndef WFC_DISABLE_CLOG
      std::clog << _name << " " << _ident << " " << _ss.str();
#endif
    }
  }
  
  logstream(const logstream& ll)
    : _name(std::move(ll._name))
    , _ident(std::move(ll._ident))
    , _ss(ll._ss.str())
    , _logger(std::move(ll._logger))
  {
  }
  
  logstream(const std::string& name, const std::string& ident, std::shared_ptr<ilogger> logger)
    : _name(name)
    , _ident(ident)
    , _logger(logger)
  {
    if ( _logger )
      _logger->initialize(_name, _ss);
  }
  
  std::string str() const
  {
    return _ss.str();
  }

  template<typename V>
  std::stringstream& operator << ( V v)
  {
    //if ( _logger )
    _ss<<v;
    return _ss;
  }

private:
  std::string _name;
  std::string _ident;
  std::stringstream _ss;
  std::shared_ptr<ilogger> _logger;
};

}
