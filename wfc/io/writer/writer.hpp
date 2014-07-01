#pragma once

//#include <wfc/io/writer/tags.hpp>
#include <wfc/io/descriptor_holder.hpp>
#include <wfc/memory.hpp>
#include <fas/aop.hpp>
#include <memory>
#include <string>
#include <iostream>

namespace wfc{ namespace io{ namespace writer{
  
struct _error_code_;

struct ad_async_write
{
  template<typename T>
  void operator()(T&,  typename T::data_ptr )
  {
    
  }
};

template<typename Tg>
struct async_write_some
{

   // ::wfc::io::data_type tmp;
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( d!= nullptr )
    {
      TRACE_LOG_MESSAGE( "ASYNC WRITE [[" << std::string(d->begin(), d->end() ) << "]]..." )
    }
    auto dd = std::make_shared<typename T::data_ptr>( std::move(d) );
    
    auto pthis = t.shared_from_this();
    auto callback =         [pthis, dd]( boost::system::error_code ec, std::size_t bytes_transferred )
        { 
          DEBUG_LOG_BEGIN("async_write_some: " << ec.message())
          typename T::lock_guard lk(pthis->mutex());
          if ( ec )
          {
            pthis->get_aspect().template get<_status_>() = false;
            pthis->get_aspect().template get<_error_code_>() = ec;
          }
          pthis->get_aspect().template get<Tg>()(*pthis, std::move(*dd), bytes_transferred);
          DEBUG_LOG_END("async_write_some: " << ec.message())
        };
        
    //auto wcallback = t.owner().wrap(callback);
  
    
    t.mutex().unlock();
    t.descriptor().async_write_some
    (
      ::boost::asio::buffer( **dd ),
      callback
      //::boost::asio::buffer( tmp   ),
      //t.owner().wrap(t.strand().wrap( t.owner().wrap(
      /*)), 
      []()
      { 
      })*/
    );
    t.mutex().lock();
  }
};



struct _write_;
struct _async_write_;
struct _incoming_;
struct _output_;


struct _create_;
struct _handler_;
struct _ready_;
struct _process_;
struct _write_some_;
struct _make_buffer_;
struct _free_buffer_;
struct _outgoing_buffer_size_;
struct _outgoing_buffer_list_;
struct _write_handler_;
struct _write_ready_;
struct _write_aborted_;
struct _write_error_;
struct _on_write_;
struct _write_more_;

struct ad_make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t, typename T::data_ptr d)
  {
    typedef typename T::data_type data_type;
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    if ( lst.empty() )
    {
      if (d==nullptr)
        return nullptr;
      lst.push_back(nullptr);
      return std::move(d);
    }
    
    if ( d != nullptr )
    {
      size_t limit = t.options().output_buffer_size;
      
      auto beg = d->begin();
      auto end = d->end();
    
      for(;beg != end; )
      {
        if ( std::distance(beg, end) > static_cast<ptrdiff_t>(limit*2) ) // Последний блок может быть [BufferSize..BufferSize*2]
        {
          lst.push_back( std::make_unique<data_type>(beg, beg + limit));
          std::advance(beg, limit);
        }
        else
        {
          lst.push_back( std::make_unique<data_type>(beg, end) );
          beg = end;
        }
      }
    }
    
    if ( lst.empty() )
      return nullptr;
    
    // TODO: отдать в список буфферов
    
    d = std::move( lst.front() );
    
    // Оставляем до free_buffer
    // lst.pop_front();
    
    return std::move(d);
  }
};

template<typename TgReady, typename TgAborted, typename TgError>
struct result_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, size_t byte_transfered)
  {
    bool status = t.get_aspect().template get< ::wfc::io::_status_>();

    if (status)
    {
      t.get_aspect().template get<TgReady>()( t, std::move(d), byte_transfered);
    }
    else 
    {
      t.get_aspect().template get< ::wfc::io::_status_>() = false;
      boost::system::error_code ec = t.get_aspect().template get< _error_code_>();
      
      TRACE_LOG_MESSAGE("WRITER error: " << ec.value() << ", " << ec.message()  )
      
      if ( ec == boost::asio::error::operation_aborted)
      {
        t.get_aspect().template get< TgAborted >()(t, ec, std::move(d) );
      }
      else
      {
        t.get_aspect().template get< TgError >()(t, ec, std::move(d) );
        t.self_stop(t, nullptr);
      }
    }
  }
};

struct ad_write
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template get<_outgoing_buffer_size_>()+=d->size();
    d = t.get_aspect().template get<_make_buffer_>()(t, std::move(d) );
    
    if ( d != nullptr )
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
  }
};

struct write_ready
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    t.get_aspect().template get<_outgoing_buffer_size_>()-=bytes_transferred;
    t.get_aspect().template gete<_on_write_>()( t, d->begin(), d->begin() + bytes_transferred );
    t.get_aspect().template get<_process_>()( t, std::move(d), bytes_transferred);
  }
};

struct write_more
{
  template<typename T>
  void operator()(T& t)
  {
    auto d = t.get_aspect().template get< _make_buffer_ >()(t, nullptr);
    if ( d != nullptr )
    {
      t.get_aspect().template get< _write_some_ >()(t, std::move(d) );
    }
  }
};

template<typename Tg>
struct ad_process
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    lst.pop_front();
    if ( d->size() != bytes_transferred )
    {
      this->process(t, std::move(d), bytes_transferred);
    }
    t.get_aspect().template get<Tg>()( t /*, bytes_transferred*/);
  }

private:
  
  template<typename T>
  void process(T& t, typename T::data_ptr d, std::size_t bytes_transferred)
  {
    typedef typename T::data_type data_type;

    size_t size = d->size() - bytes_transferred;
    size_t limit = t.options().output_buffer_size;
    auto& lst = t.get_aspect().template get<_outgoing_buffer_list_>();
    
    auto beg = d->rbegin();
    auto end = d->rbegin() + size;

    for(;beg != end; )
    {
      if ( std::distance(beg, end) > static_cast<ptrdiff_t>(limit*2) ) // Последний блок может быть [BufferSize..BufferSize*2]
      {
        lst.push_front( std::make_unique<data_type>( (beg + limit).base(), beg.base()));
        std::advance(beg, limit);
      }
      else
      {
        lst.push_front( std::make_unique<data_type>(end.base(), beg.base() ) );
        beg = end;
      }
    }
  }
};


typedef std::list<  ::wfc::io::basic::data_ptr> data_list;

template< template<typename> class AsyncWriteSome/*, typename TgOutgoing = _incoming_, typename TgOutput = _output_*/ >
struct aspect: fas::aspect<
  fas::value< _error_code_, boost::system::error_code>,
  fas::alias<_incoming_, _write_>, 
  fas::advice< _write_some_,    AsyncWriteSome<_write_handler_> >,      /// async_read_some - Вынести 
  fas::advice< _write_handler_, result_handler<_write_ready_, _write_aborted_, _write_error_> >,
  fas::advice< _write_, ad_write>, 
  fas::advice< _make_buffer_, ad_make_buffer>, 
  fas::value< _outgoing_buffer_list_, data_list>, 
  fas::value< _outgoing_buffer_size_, size_t>, 
  fas::advice< _write_ready_, write_ready>, 
  fas::advice<_process_, ad_process<_write_more_> >,
  fas::advice<_write_more_, write_more>, 
  fas::stub<_write_aborted_>, 
  fas::stub<_write_error_>
  
  

  /*
  fas::advice<_ready_, ad_ready>,
  fas::advice<_handler_, ad_handler>,
  fas::advice<_make_buffer_, ad_make_buffer>,
  fas::advice<_process_, ad_process>,
  fas::advice<_write_, ad_write>,
  fas::alias< _incoming_, _handler_>,
  
  fas::value< _output_buffer_size_, size_t>,
  fas::value< _outgoing_buffer_list_, data_list>,
  */



  /*
  fas::advice< _make_buffer_, make_buffer>,
  fas::value< _is_started_, bool>,
  
  fas::advice< _set_transfer_handler_, set_transfer_handler<TgOutput> >,
  
  fas::group<  ::wfc::io::_create_, _set_transfer_handler_ >,
  
  // start
  fas::advice<  ::wfc::io::_start_, dispatch<_start_> >,
  fas::advice< _start_, event< _read_more_, _on_start_> >, 
  
  // loop
  fas::advice< _read_more_,    read_more<_read_some_> >,
  fas::advice< _read_some_,    AsyncReadSome<_read_handler_> >,      /// async_read_some - Вынести 
  fas::advice< _read_handler_, result_handler<_read_ready_, _read_aborted_, _read_error_> >,
  
  // events
  fas::advice< _read_ready_,   ready_handler< _readed_,   _on_read_> > ,
  fas::advice< _read_aborted_, error_handler< _aborted_,  _on_aborted_> >,
  fas::advice< _read_error_,   error_handler< _error_,    _on_error_> >,
    
  // ready
  fas::advice< _readed_, readed<TgOutgoing, _read_more_> >,         /// _outgoing_ - вынести или добавить handle??
  // fas::alias< TgOutgoing, _incoming_ >,   // заглушка для переопределения 
  fas::alias< _incoming_, _user_handler_>,
  fas::advice< _user_handler_, user_handler< TgOutput > >,
  fas::advice< TgOutput, output_stub>,
  fas::advice< _aborted_, output_stub>,
  fas::advice< _error_, output_stub>
  */

>{};

template<typename Descriptor,  typename TgOutgoing = _incoming_ >
struct stream: fas::aspect<
  //fas::stub<TgOutgoing>, 
  fas::type<  ::wfc::io::_descriptor_type_, Descriptor>,
  aspect<async_write_some/*, TgOutgoing*/>
  /*
  fas::advice< _write_, ad_write>,
  fas::advice< _async_write_, ad_async_write>
  */
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
  fas::advice<  ::wfc::io::_options_type_, options >,
  aspect< async_read_some2, TgOutgoing,  _read_incoming_>
  */
>
{};
  

struct basic_options
{
  size_t output_buffer_size = 1024*8;
};

struct options
  :  ::wfc::io::basic::options
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
  
  void stop(std::function<void()> finalize)
  {
    super::stop(*this, finalize);
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
