#pragma once



namespace wfc{ namespace inet{ namespace conn{ namespace stream{


template<typename Base = fas::empty_type/*empty_context*/>
struct basic_context: wfc::inet::conn::basic_context<Base>
{
  size_t outgoing_warning;
  size_t outgoing_limit;
  bool shutdown;
  basic_context()
    : wfc::inet::conn::basic_context<Base>()
    , outgoing_warning(0)
    , outgoing_limit(0)
    , shutdown(false)
  {}
};

}}}}
