#pragma once

#include <wfc/module/basic_component.hpp>

namespace wfc{

template< typename Name, typename Instance, typename DomainJson>
class multiton: public basic_component<Name, Instance, DomainJson, false> {};

}
