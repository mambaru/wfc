#pragma once

#include <wfc/io/tags.hpp>
#include <wfc/io/types.hpp>
#include <wfc/io/reader/tags.hpp>
#include <wfc/io/async_read_some.hpp>

#include <wfc/io/common/fork.hpp>

#include <wfc/memory.hpp>
#include <fas/aop.hpp>

#include <wfc/logger.hpp>
#include <wfc/callback/callback_status.hpp>

#include <list>

namespace wfc{ namespace io{ namespace reader{ 

struct _is_started_;

template<typename Tg>
struct dispatch
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "dispatch " << std::endl;
    t.dispatch( [&t]() {
      std::cout << "dispatch start" << std::endl;
      t.get_aspect().template get<_is_started_>() = true;
      t.get_aspect().template gete<Tg>()(t);
      std::cout << "dispatch finish" << std::endl;
    });
  }
};

template<typename Tg, typename TgEvent>
struct event
{
  template<typename T>
  void operator()(T& t)
  {
    t.get_aspect().template gete<TgEvent>()(t);
    t.get_aspect().template get<Tg>()(t);
  }
};

template<typename Tg, typename TgEvent>
struct ready_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    t.get_aspect().template gete<TgEvent>()(t, d->begin(), d->end() );
    t.get_aspect().template get<Tg>()(t, std::move(d) );
  }
};

struct _make_buffer_;
struct _read_some_;
struct _read_more_;

struct make_buffer
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    return std::make_unique<typename T::data_type>( t.options().input_buffer_size );
  }
};

template<typename Tg>
struct read_more
{
  template<typename T>
  void operator()(T& t)
  {
    std::cout << "read more {" << std::endl;
    auto d = t.get_aspect().template get< _make_buffer_ >()(t);
    t.get_aspect().template get< Tg >()(t, std::move(d) );
    std::cout << "}read more" << std::endl;
  }
};

struct _result_ready_;
struct _result_error_;
struct _result_aborted_;


template<typename TgReady, typename TgAborted, typename TgError>
struct result_handler
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    bool status = t.get_aspect().template get<wfc::io::_status_>();

    if (status)
    {
      t.get_aspect().template get<TgReady>()( t, std::move(d) );
    }
    else 
    {
      boost::system::error_code ec = t.get_aspect().template get<wfc::io::_error_code_>();
      if ( ec == boost::asio::error::operation_aborted)
      {
        t.get_aspect().template get< TgAborted >()(t, ec, std::move(d) );
      }
      else
      {
        t.get_aspect().template get< TgError >()(t, ec, std::move(d) );
      }
    }
  }
};

template<typename Tg, typename TgEvent>
struct error_handler
{
  template<typename T>
  void operator()(T& t, boost::system::error_code ec, typename T::data_ptr d)
  {
    t.get_aspect().template gete<TgEvent>()(t, ec );
    t.get_aspect().template get<Tg>()(t, std::move(d) );
  }
};

/*
struct readed
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    if ( t.get_aspect().template get<_is_started_>() )
  }
}
*/

template<typename TgMain, typename TgSecond>
struct readed
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "readed! " << std::endl;
    t.get_aspect().template get< TgMain >()(t, std::move(d));
    
    if ( t.get_aspect().template get<_is_started_>() )
    {
      std::cout << "readed! is started! " << std::endl;
      t.get_aspect().template get< TgSecond >()(t);
    }
  }
};



template<typename TgResult>
struct user_handler
{
  template<typename T>
  void operator()( T& t, typename T::data_ptr d)
  {
    auto handler = t.options().handler;
    if ( handler == nullptr )
    {
      std::cout << "user_handler null " << std::endl;
      t.get_aspect().template get<TgResult>()(t, std::move(d) );
    }
    else
    {
      std::cout << "user_handler shared_from_this{ " << std::endl;
      t.shared_from_this();
      std::cout << "}user_handler shared_from_this " << std::endl;
      std::cout << "user_handler { " << std::endl;
      handler(
        std::move(d), 
        t.shared_from_this(), 
        t.callback([&t](typename T::data_ptr d){
          t.get_aspect().template get<TgResult>()(t, std::move(d) );
        })
      );
      std::cout << "} user_handler " << std::endl;
      
    }
  }
};

struct output_stub
{
  template<typename T>
  void operator()( T& , typename T::data_ptr )
  {
  }
};


struct _start_;
struct _output_;
struct _on_start_;
struct _read_handler_;
struct _read_ready_;
struct _read_aborted_;
struct _read_error_;
struct _readed_;
struct _aborted_;
struct _error_;
struct _on_read_;
struct _on_error_;
struct _on_aborted_;
struct _user_handler_;

  
struct basic_options
{
  wfc::io::handler handler = nullptr;
  size_t input_buffer_size = 8096;
  std::function<callback_status()> not_alive = nullptr;
};

/*
 * Унивирсальный аспект
 */

template< template<typename> class AsyncReadSome, typename TgOutgoing = _incoming_, typename TgOutput = _output_ >
struct aspect: fas::aspect<
  fas::advice< _make_buffer_, make_buffer>,
  fas::value< _is_started_, bool>,
  
  // start
  fas::advice< wfc::io::_start_, dispatch<_start_> >,
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
>{};

struct options: basic_options
{
  std::function<callback_status()> not_alive = nullptr;
};

typedef std::list< wfc::io::data_ptr> data_list;
typedef std::list< wfc::io::callback> callback_list;
struct _sync_read_some_;
struct _sync_read_more_;
struct _read_incoming_;
struct _callback_list_;
struct _outgoing_list_;

template<typename TgAsyncReadMore>
struct read_incoming
{
  template<typename T>
  void operator()(T& t, typename T::data_ptr d)
  {
    std::cout << "read_incoming!!!" << std::endl;
    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    if ( d!=nullptr)
      lst.push_back( std::move(d) );
    
    auto& clb = t.get_aspect().template get<_callback_list_>();
    while ( !clb.empty() &&  !lst.empty() )
    {
      auto d2 = std::move( lst.front() );
      lst.pop_front();
      auto handler = clb.front();
      clb.pop_front();
      if ( handler != nullptr )
        handler( std::move(d2) );
    }
    
    if ( !clb.empty() )
      t.get_aspect().template get< TgAsyncReadMore >()(t);
    
    std::cout << "read_incoming!!! " << lst.size() << std::endl;

  }
};

struct _read_;
template<typename TgReadMore>
struct read
{
  template<typename T>
  typename T::data_ptr operator()(T& t)
  {
    std::cout << "READ{" << std::endl;
    if ( !t.status() )
    {
      std::cout << "status1! " << t.error_code().message() << std::endl;
      return nullptr;
    }
    
    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    if ( lst.empty() && t.status() )
      t.get_aspect().template get< TgReadMore >()(t);

    if ( !t.status() )
    {
      std::cout << "status2! " << t.error_code().message() << std::endl;
      return nullptr;
    }

    
    typename T::data_ptr d = nullptr;
    if ( !lst.empty() )
    {
      std::cout << "READ OK " << std::string(d->begin(), d->end()) <<std::endl;
      d = std::move( lst.front() );
      lst.pop_front();
    }
    std::cout << "}READ " <<  std::endl;
    return std::move(d);
  }
};

struct _async_read_;

template<typename TgAsyncReadMore>
struct async_read
{
  template<typename T>
  void operator()(T& t, wfc::io::callback handler)
  {
    if ( !t.status() )
    {
      handler(nullptr);
      return;
    }

    auto& lst = t.get_aspect().template get<_outgoing_list_>();
    auto& clb = t.get_aspect().template get<_callback_list_>();
    
    if ( !lst.empty() && clb.empty() )
    {
      auto d = std::move( lst.front() );
      lst.pop_front();
      handler( std::move(d) );
    }
    else
    {
      clb.push_back(handler);
      t.get_aspect().template get< TgAsyncReadMore >()(t);
    }
  }
};


template<typename Descriptor,  typename TgOutgoing = _incoming_ >
struct stream: fas::aspect<
  fas::type< wfc::io::_descriptor_type_, Descriptor>,
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
>
{};


struct _log_aborted_;
struct ad_log_aborted
{
  template<typename T>
  void operator()(T&, boost::system::error_code ec)
  {
    COMMON_LOG_WARNING( "READER aborted: " << ec.message() )
  }
};

struct _log_error_;
struct ad_log_error
{
  template<typename T>
  void operator()(T&, boost::system::error_code ec)
  {
    COMMON_LOG_ERROR( "READER error: " << ec.message() )
  }
};


/// Еррор лог аспект

struct error_log: fas::aspect<
  fas::advice<_log_error_, ad_log_error>,
  fas::advice<_log_aborted_, ad_log_aborted>,
  fas::group<_on_error_, _log_error_>,
  fas::group<_on_aborted_, _log_aborted_>
>{};



}}}
