#pragma once

#include <wfc/module/basic_object.hpp>

namespace wfc{

template< typename Name, typename Instance, typename DomainJson>
class singleton: public basic_object<Name, Instance, DomainJson, true> {};

}
