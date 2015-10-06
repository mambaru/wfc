#pragma once 

#include <wfc/jsonrpc/interface_implementation.hpp>
#include <wfc/jsonrpc/basic_domain.hpp>
#include <iow/jsonrpc/handler.hpp>

namespace wfc{ namespace jsonrpc{

template<typename MethodList, template<typename> class Impl = interface_implementation>
class gateway
  : public Impl< basic_domain< typename MethodList::interface_type, ::iow::jsonrpc::handler< Impl<MethodList> > > >
{
public:
};

}}