#pragma once

//#include <wfc/io/writer/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>

namespace wfc{ namespace io{ namespace writer{
  
struct ad_write
{
  template<typename T>
  void operator()(T&, typename T::data_ptr )
  {
    
  }
};

struct ad_async_write
{
  template<typename T>
  void operator()(T&,  typename T::data_ptr )
  {
    
  }
};

  
struct _write_;
struct _async_write_;
struct _incoming_;
struct _output_;

template< template<typename> class AsyncWriteSome, typename TgOutgoing = _incoming_, typename TgOutput = _output_ >
struct aspect: fas::aspect<
>{};

template<typename Descriptor,  typename TgOutgoing = _incoming_ >
struct stream: fas::aspect<
  fas::type< wfc::io::_descriptor_type_, Descriptor>,
  fas::advice< _write_, ad_write>,
  fas::advice< _async_write_, ad_async_write>
  /*
  ,
  fas::value<  _outgoing_list_, data_list>,
  fas::value<  _callback_list_, callback_list>,
  
  fas::alias< _output_, _read_incoming_>,
  fas::advice< _read_incoming_, read_incoming<_read_more_> >,
  
  // sync read
  fas::advice< _sync_read_some_,  read_some<_read_handler_> >,
  fas::advice< _sync_read_more_,  read_more<_sync_read_some_> >,
  // methods
  fas::advice< _read_, read<_sync_read_more_> >,
  fas::advice< _async_read_, async_read<_read_more_> >,
  fas::advice< wfc::io::_options_type_, options >,
  aspect< async_read_some2, TgOutgoing,  _read_incoming_>
  */
>
{};
  

struct basic_options
{
};

struct options
  : wfc::io::basic::options
  , basic_options
{
  
};

  
struct writer_aspect: fas::aspect
<
  fas::stub<_write_>,
  fas::stub<_async_write_>,
  fas::stub<_options_type_, basic_options>
>
{};
  
template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
class writer_base
  : public descriptor_holder<A, AspectClass>
  , public std::enable_shared_from_this< writer_base<A, AspectClass> >
{
public:
  
  typedef writer_base<A, AspectClass> self;
  typedef descriptor_holder<A, AspectClass> super;
  
  typedef typename super::data_type data_type;
  typedef typename super::descriptor_type descriptor_type;
  typedef typename super::options_type options_type;
  typedef std::unique_ptr<data_type> data_ptr;
  
public:

  writer_base(const writer_base& ) = delete;
  void operator = (const writer_base& conf) = delete;

  writer_base(descriptor_type&& desc, const options_type& conf)
    : super( std::move(desc), conf)
  {
    super::create(*this);
  }
  
  void start()
  {
    super::start(*this);
  }
  
  void stop()
  {
    super::stop(*this);
  }
  
  bool status() const
  {
    return this->get_aspect().template get<_status_>();
  }
  
  void write(data_ptr d)
  {
    return this->get_aspect().template get< _write_ >()(*this, std::move(d));
  }

  void async_write(data_ptr d)
  {
    this->get_aspect().template get< _async_write_ >()(*this, std::move(d));
  }

private:

};

template<typename A = fas::aspect<>, template<typename> class AspectClass = fas::aspect_class >
using writer = writer_base< typename fas::merge_aspect< A, writer_aspect>::type, AspectClass>;

}}}
