#pragma once

#include <wfc/module/basic_object.hpp>

namespace wfc{

template< typename Name, typename Instance, typename DomainJson>
class multiton: public basic_object<Name, Instance, DomainJson, false> {};

}
