#pragma once

#include <wfc/io/writer/write/ad_write.hpp>
#include <wfc/io/writer/write/ad_incoming.hpp>
#include <wfc/io/writer/write/tags.hpp>
#include <wfc/io/writer/tags.hpp>

#include <fas/aop.hpp>


namespace wfc{ namespace io{ namespace writer{ namespace write{ 

// TODO: сделать mutex



typedef fas::type_list_n<
  fas::value<  _writed_size_, size_t>,
  fas::advice< _write_, ad_write>,
  fas::advice< _incoming_, ad_incoming >
>::type advice_list;


struct aspect: fas::aspect
< 
  advice_list,
  fas::alias< wfc::io::writer::_write_, _write_ >
>
{};

}}}}
