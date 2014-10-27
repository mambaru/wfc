#pragma once

#include <wfc/gateway/provider/provider_base.hpp>
#include <wfc/gateway/provider_config.hpp>
#include <wfc/memory.hpp>
#include <wfc/logger.hpp>
#include <wfc/io_service.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <map>
#include <set>

namespace wfc{ namespace gateway{ 

template<typename Itf>  
class sequenced_provider
  : public basic_provider<Itf, sequenced_provider, std::recursive_mutex>
{
  typedef sequenced_provider<Itf> self;
  typedef basic_provider<Itf, sequenced_provider, std::recursive_mutex> super;
  typedef std::chrono::high_resolution_clock clock_t;
  typedef clock_t::time_point time_point_t;

  const size_t null_id = static_cast<size_t>(-1);

public:
  
  typedef typename super::interface_type interface_type;
  typedef typename super::mutex_type mutex_type;
  typedef std::pair<size_t, size_t> clicall_pair;
  typedef std::function<clicall_pair()> post_function;
  typedef ::boost::asio::deadline_timer deadline_timer;
  typedef std::shared_ptr<deadline_timer> timer_ptr;
  typedef std::tuple<size_t, post_function, timer_ptr> clipost;
  typedef std::map<size_t, clipost> callclipost_map;
  
  typedef std::set<clicall_pair> clicall_set;
  typedef std::deque< post_function > delayed_queue;

  sequenced_provider( ::wfc::io_service& io, const provider_config& conf)
    : super(conf)
    , _io_service(io)
    , _io_work(io)
    //, _deadline_timer(io)
    // , _wait_client_id(null_id)
    // , _wait_call_id(0)
    , _call_id_counter(0)
    , _post_count(0)
    , _recall_count(0)
    , _orphan_count(0)
    , _drop_count(0)
  {
  }
  
  size_t post_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _post_count;
  }

  size_t orphan_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _orphan_count;
  }

  size_t recall_count() const 
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _recall_count;
  }

  size_t drop_count() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _drop_count;
  }

  // Размер очереди
  size_t queue_size() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _queue.size();
  }

  // Число запросов ожидающих подверждения
  size_t wait_count() const
  {
    ::wfc::read_lock<mutex_type> lk( super::_mutex );
    return _clicall.size();
  }

  
  virtual void startup(size_t client_id, std::shared_ptr<interface_type> ptr )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );

    super::startup_(client_id, ptr);
    /*_recall_count += */this->process_queue_();
    
    /*if ( _wait_client_id == null_id )
    {
      _recall_count += this->process_queue_();
    }
    */
  }


  virtual void shutdown(size_t client_id)
  {
    std::lock_guard<mutex_type> lk( super::_mutex );

    super::shutdown_(client_id);
    
    auto beg = _clicall.lower_bound({client_id, 0});
    auto end = _clicall.upper_bound({client_id, null_id});
    for (auto itr = beg; itr!=end; )
    {
      if ( itr->first != client_id )
        break;
      auto itr2 = _callclipost.find( itr->second );
      if ( itr2 != _callclipost.end() )
      {
        _queue.push_front( std::get<1>(itr2->second) );
        _callclipost.erase(itr2);
       _clicall.erase(itr++);
      }
      else
      {
        std::cout << "FUCK1 " <<  _clicall.size() << " " << _callclipost.size() << std::endl;
        std::cout << "FUCK2 " <<  itr->second << " " << _callclipost.begin()->first << std::endl;
        abort();
        itr++;
        continue;
      }
      //_clicall.erase((itr++).base());

      DAEMON_LOG_WARNING( 
        "provider(mode=sequenced): закрытие соединения при ожидании подверждения удаленного вызова. "
        "Информация о выполнении вызова отсутствует.")
      // ++_orphan_count; // счетчик проблемных вызовов (или recall???)
      ++_recall_count; // !!!Счетчик будующих повторных вызовов

    }
    
    /*_recall_count += */this->process_queue_();
    /*
    if ( _wait_client_id == client_id )
    {
      DAEMON_LOG_WARNING( 
        "provider(mode=sequenced): закрытие соединения при ожидании подверждения удаленного вызова. "
        "Информация о выполнении вызова отсутствует. ")
      _wait_client_id = null_id;
      _wait_call_id = 0;
      _recall_count += this->process_queue_();
    }
    */
  }
  
  template<typename Req, typename Callback, typename... Args>
  void post( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    std::lock_guard<mutex_type> lk( super::_mutex );
    
    if ( !super::_conf.enabled )
      return;
    
    if ( super::_conf.wait_limit==0 && super::_conf.queue_limit==0 )
    {
      // Простой режим
      if ( auto cli = super::get() )
      {
        (cli.get()->*mem_ptr)( std::move(req), std::move(callback), std::forward<Args>(args)... );
      }
      else
      {
        //++_current_drop_count;
        std::cout << "-drop-1-" << std::endl;
        ++_drop_count;
      }
      return;
    }

    // Далее нужна обертка
    post_function f = this->wrap_(mem_ptr, std::move(req), std::move(callback), std::forward<Args>(args)...); 
    
    if ( _clicall.size() < super::_conf.wait_limit  )
    {
      auto result = f();
      auto itr = _callclipost.find(result.second);
      if ( itr != _callclipost.end() )
        std::get<1>(itr->second) = f;
      else
      {
        std::cout << "wait limit! " <<  super::_conf.wait_limit << std::endl;
        std::cout << "_clicall.size() " <<  _clicall.size() << std::endl;
        //abort();
        _queue.push_back( f );
      }

    }
    else if ( _queue.size() < super::_conf.queue_limit )
    {
      _queue.push_back( f );
    }
    else
    {
      /*
      std::cout << "-drop-2- " << super::_conf.wait_limit << " " << super::_conf.queue_limit << std::endl;
      std::cout << "-drop-3- " << _clicall.size() << " " << _queue.size() << std::endl;
      */
      ++_drop_count;
      // Потерянный вызов (TODO warning в LOG)
    }
    /*
    _queue.push( f );
    
    if ( _wait_client_id != null_id )
    {
      return;
    }
      
    if ( _queue.size() != 1 )
    {
      // Проверить наличие коннектов. Если есть то fail
      return;
    }
    
    f();
    */
  }

private:
  
  static void deadline_( 
    std::shared_ptr<self> pthis,
    size_t client_id, 
    size_t call_id)
  {
    std::lock_guard<mutex_type> lk( pthis->_mutex );
    std::cout << "deadline_ client_id=" << client_id << " call_id=" << call_id << " wait size=" << pthis->_callclipost.size() << std::endl;
    auto call_itr = pthis->_callclipost.find(call_id);
    if ( call_itr == pthis->_callclipost.end() )
    {
      // был shutdown и инфу удалили
      return;
    }
    
    auto cli_itr = pthis->_clicall.find( { std::get<0>(call_itr->second), call_itr->first});
    if ( cli_itr == pthis->_clicall.end() )
    {
      std::cout << "abort deadline_ " << std::get<0>(call_itr->second) << ":" << call_itr->first << std::endl;
      abort();
    }
    std::cout << "deadline_ " <<  client_id << " " << cli_itr->first << std::endl;
    if ( client_id != cli_itr->first )
    {
      // abort() ???
      return;
    }
    std::cout << "deadline_ 3" << std::endl;
    // TODO: в лог
    pthis->_callclipost.erase(call_itr);
    pthis->_clicall.erase(cli_itr);
    ++pthis->_drop_count;
    /*
    if (client_id != pthis->_wait_client_id 
        || call_id != pthis->_wait_call_id)
    {
      // TODO: log
      return;
    }
    
    ++pthis->_drop_count;
    // TODO: log
    pthis->result_ready_();
    */
  }
    

  
  template<typename Req, typename Callback, typename... Args>
  static clicall_pair send_( 
    std::shared_ptr<self> pthis,
    size_t call_id,
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    std::shared_ptr<Req> req, 
    Callback callback, 
    Args... args
  )
  {
    // Здесь инкремент call_id
    // TODO: проверку что режим соблюдаеться
    clicall_pair result = {pthis->null_id, call_id};
    //size_t client_id = null_id;
    if ( auto cli = pthis->get_( result.first ) )
    {
      /*auto cli_itr =*/ pthis->_clicall.insert({result.first, call_id});
      auto call_itr = pthis->_callclipost.insert({call_id, clipost{result.first, nullptr, nullptr}}).first;
      std::shared_ptr<deadline_timer> timer;
      if (pthis->_conf.wait_timeout_ms != 0 )
      {
        timer = std::make_shared<deadline_timer>(pthis->_io_service);
        std::get<2>(call_itr->second) = timer;
      }
      //pthis->_mutex.unlock();
      try
      {
        // Копируем запрос, т.к. возможен повторный вызов
        auto req_for_send = std::make_unique<typename Req::element_type>(**req);
        if (timer)
        {
          
          timer->expires_from_now(boost::posix_time::milliseconds(pthis->_conf.wait_timeout_ms));
          timer->async_wait( std::bind(
              &sequenced_provider<Itf>::deadline_, 
              pthis, 
              result.first, 
              call_id
          ) );
        }
        std::cout << "{{{" << std::endl;
        ++(pthis->_post_count);
        (cli.get()->*mem_ptr)( std::move(req_for_send), std::move(callback), std::forward<Args>(args)... );
        std::cout << "}}} " << pthis->_post_count << std::endl;
      } 
      catch ( ... )
      {
        std::cout << "2 sequenced_provider::send_ unhandled exception" << std::endl;
        DAEMON_LOG_FATAL("2 sequenced_provider::send_ unhandled exception")
        abort();
      }
      //pthis->_mutex.lock();
      //ready = true;
    }
    else
    {
      std::cout << "FUCK" << std::endl;
    }
    return result;
    
    /*
    if ( pthis->_wait_client_id != null_id )
    {
      DAEMON_LOG_FATAL("" << "Logic error " << pthis->_wait_client_id << " thread id=" << *(reinterpret_cast<size_t*>(pthis.get())))
      abort();
    }
     
    bool ready = false;
    // вызов только из критической секции
    pthis->_wait_client_id = null_id;
    if ( auto cli = pthis->get_( pthis->_wait_client_id ) )
    {
      size_t client_id = pthis->_wait_client_id;
      pthis->_wait_call_id = call_id;
      pthis->_mutex.unlock();
      
      try
      {
        // Копируем запрос, т.к. возможен повторный вызов
        auto req_for_send = std::make_unique<typename Req::element_type>(**req);
        if (pthis->_conf.timeout_ms != 0 )
        {
          pthis->_deadline_timer.expires_from_now(boost::posix_time::milliseconds(pthis->_conf.timeout_ms));
          pthis->_deadline_timer.async_wait( std::bind(
              &sequenced_provider<Itf>::deadline_, 
              pthis, 
              client_id, 
              call_id
          ) );
        }
        (cli.get()->*mem_ptr)( std::move(req_for_send), std::move(callback), std::forward<Args>(args)... );
      } 
      catch ( ... )
      {
        DAEMON_LOG_FATAL("2 sequenced_provider::send_ unhandled exception")
        abort();
      }
      pthis->_mutex.lock();
      ready = true;
    }
    else
    {
      pthis->_wait_client_id = null_id;
    }
    return ready;
    */
  }
    
  template<typename Req, typename Callback, typename... Args>
  post_function wrap_( 
    void (interface_type::*mem_ptr)(Req, Callback, Args... args), 
    Req req, 
    Callback callback, 
    Args... args
  )
  {
    if (req == nullptr)
      std::cout << "nullptr!" << std::endl;
    auto preq = std::make_shared<Req>( std::move(req) );
    auto pthis = super::shared_from_this();
    ++this->_call_id_counter;
    
    //TODO: после отправки обновить мапу функцией отправки!
    
    return std::bind(
      &sequenced_provider<Itf>::send_<Req, Callback, Args...>,
      pthis,
      this->_call_id_counter,
      mem_ptr, 
      preq, 
      this->wrap_callback_(callback), 
      std::forward<Args>(args)...
    );
  }
  
  void result_ready_()
  {
    this->pop_();
    this->process_queue_();
  }
    
  template<typename Resp, typename... Args>
  static void mt_confirm_( 
    std::shared_ptr<self> pthis,
    size_t call_id,
    std::function<void(Resp, Args...)> callback,
    std::shared_ptr<Resp> resp, 
    Args... args
  )
  {
    {
      std::lock_guard<mutex_type> lk(pthis->_mutex); 
      auto call_itr = pthis->_callclipost.find(call_id);
      if ( call_itr == pthis->_callclipost.end() )
      {
        ++pthis->_orphan_count;
        return;
      }
      
      if ( auto timer = std::get<2>(call_itr->second) )
      {
        timer->cancel();
        std::get<2>(call_itr->second)=nullptr;
      }
      pthis->_callclipost.erase(call_itr);
      pthis->_clicall.erase({std::get<0>(call_itr->second),call_id});
      
      //std::cout << "confirm!" << std::endl;
      // TODO: stop timer, и в отдельную функцию
      
      pthis->result_ready_();
      /*
      if ( pthis->_wait_call_id != call_id )
      {
        ++pthis->_orphan_count;
        return;
      }
      pthis->result_ready_();
    */
    }
      
    if ( callback!=nullptr )
      callback( std::move(*resp), std::forward<Args>(args)... );
  }

  
  template<typename Resp, typename ...Args>
  std::function<void(Resp, Args...)>
  wrap_callback_(std::function<void(Resp, Args...)> callback) 
  {
    auto call_id = this->_call_id_counter;
    auto pthis = this->shared_from_this();

    auto callback_fun = &sequenced_provider<Itf>::mt_confirm_<Resp, Args...>;
    return [callback_fun, pthis, call_id, callback](Resp resp, Args... args)
    {
      auto presp = std::make_shared<Resp>(std::move(resp) );
      pthis->_io_service.post( std::bind(
        callback_fun,
        pthis,
        call_id,
        callback,
        presp,
        std::forward<Args>(args)...
      ) );
    };
  }
  
  void pop_()
  {
    /*
    _queue.pop();
    _wait_client_id = null_id;
    _wait_call_id = 0;
    */
  }
  

  int process_queue_()
  {
    int count = 0;
    while ( !_queue.empty() )
    {
      if ( _callclipost.size() >= super::_conf.wait_limit )
        break;
      //std::cout << "pr " << _callclipost.size() << std::endl;

      auto f = _queue.front();
      if ( f==nullptr )
      {
        std::cout << "if ( f==nullptr )" << std::endl;
        abort();
      }
      auto result = f();
      if ( result.first == null_id )
        break;
      _queue.pop_front();
      auto itr = _callclipost.find(result.second);
      if ( itr != _callclipost.end() )
        std::get<1>(itr->second) = f;
      else
        abort();
      if ( super::_conf.wait_limit == 0 )
        continue;
    }
    /*
    
    while ( !_queue.empty() )
    {
      _queue.front()();
      ++result;
      if ( super::_conf.mode == provider_mode::sequenced )
        break;
    }
    
    */
    return count;
  }

private:
  ::wfc::io_service& _io_service;
  ::wfc::io_service::work _io_work;
  // ::boost::asio::deadline_timer _deadline_timer;
  //size_t _wait_client_id;
  //size_t _wait_call_id;
  size_t _call_id_counter;
  callclipost_map _callclipost;
  clicall_set _clicall;
  delayed_queue _queue;
  
  size_t _post_count;
  // Повторные вызовы
  size_t _recall_count;
  // Потерянные вызовы
  size_t _orphan_count;
  // сброшенные по timeout
  size_t _drop_count;
};

}}
