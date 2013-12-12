#include <wfc/logger/logstream.hpp>
#include <iostream>

using namespace ::wfc;

class logger:public ilogger
{
public:
  virtual ~logger(){}
  //virtual void initialize(const std::string& name, std::stringstream& str) = 0;
  //virtual void write(const std::string& name, const std::string& ident,  const std::string& str) = 0;
  
  virtual void initialize(const std::string& name, std::stringstream& str) {};
  virtual void write(const std::string& name, const std::string& ident, const std::string& s) { str=s;}
  std::string str;
};

int main()
{
  logstream ls1("-", "-", nullptr);
  ls1 <<  "test";
  if ( !ls1.str().empty() )
    return -1;

  logstream ls2("-", "-", std::make_shared<logger>());
  ls2 <<  "test";
  if ( ls2.str() != "test" )
    return -1;

  auto l = std::make_shared<logger>();
  {
    logstream ls3("-", "-",  l);
    ls3 <<  "test" << 1 <<  std::endl;
    if ( ls3.str() != "test1\n" )
      return -1;
  }
  if ( l->str != "test1\n" )
    return -1;

  return 0;
}
