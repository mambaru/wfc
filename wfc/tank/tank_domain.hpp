#pragma once
#include <wfc/tank/tank_config.hpp>
#include <wfc/tank/igun.hpp>
#include <wfc/tank/logger.hpp>
#include <wflow/wflow.hpp>
#include <fas/typemanip/empty_type.hpp>
#include <wfc/domain_object.hpp>
#include <iostream>

namespace wfc{

template< typename I, typename Opt = tank_config<> >
class tank_domain
  : public domain_object<iinterface, Opt, stat_tank_config>
  , public std::enable_shared_from_this< tank_domain<I, Opt> >
{
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point_t;

  struct bang_stat
  {
    size_t wait_counter = 0;
    size_t request_counter = 0;
    size_t response_counter = 0;
    size_t bad_check_counter = 0;
    size_t bad_gateway_counter = 0;
    time_point_t start_time;
    time_point_t finish_time;

    time_t get_interval() const
    {
      return std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    }

    size_t get_rate() const
    {
      return response_counter * 1000000 / static_cast<size_t>(this->get_interval());
    }

  };

public:
  typedef std::recursive_mutex mutex_type;
  typedef tank_domain<I, Opt> self;
  typedef igun<I, Opt> gun_interface;
  typedef domain_object<iinterface, Opt, stat_tank_config> super;
  typedef typename super::options_type options_type;
  typedef typename super::timer_id_t timer_id_t;

  virtual void configure() override
  {
    _options = this->options();
    _gunfire_counter = _options.gunfire_total;
  }

  virtual void initialize() override
  {
    _gun =  this->template get_object<gun_interface>("wonderwaffel", this->name() );
    _target = this->template get_target<I>(_options.target);
    if ( _gun != nullptr )
    {
      _gun->initialize(_target, _options);
    }
  }

  virtual void start() override
  {
    if (auto pstat = this->get_statistics() )
      _meter = pstat->create_time_meter("time");

    this->get_workflow()->safe_post( std::chrono::milliseconds(_options.start_delay_ms), std::bind(&self::create_timer, this) );
  }

  virtual void stop() override
  {
    if ( _total_stat.finish_time == time_point_t() )
      _total_stat.finish_time = std::chrono::high_resolution_clock::now();
    _total_stat.wait_counter = _bang_stat.wait_counter;
    this->log_stat(_total_stat);
  }

  void create_timer()
  {
    TANK_LOG_MESSAGE("---------------TANK START------------------" )
    _bang_stat = bang_stat();
    _bang_stat.start_time = std::chrono::high_resolution_clock::now();
    _total_stat.start_time = std::chrono::high_resolution_clock::now();
    if ( gunfire() )
    {
      this->get_workflow()->create_timer(std::chrono::microseconds(_options.gunfire_mks), std::bind(&self::gunfire, this) );
      this->get_workflow()->create_timer(std::chrono::milliseconds(_options.statistics_log_ms), std::bind(&self::show_stat, this) );
    }
    else{
      show_stat();
    }
  }

  bool gunfire()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    if ( this->global_stop_flag() )
      return false;

    if ( (_gunfire_counter == 0 && _bang_stat.wait_counter == 0) || _total_stat.bad_check_counter!=0)
    {
      _total_stat.finish_time = std::chrono::high_resolution_clock::now();

      TANK_LOG_MESSAGE("---------------TANK FINISH--------------------")
      wfc_exit();
      return false;
    }

    for (size_t i = 0; 
        i  < _options.gunfire_size 
          && _bang_stat.wait_counter < _options.gunfire_max_waits
          && _gunfire_counter != 0;
          ++i)
    {
      --_gunfire_counter;
      ++_bang_stat.wait_counter;
      ++_bang_stat.request_counter;
      ++_total_stat.request_counter;
      auto tp = _meter.create_shared(1u);
      _gun->bang([this, tp](bool good_gateway, bool status)
      {
        std::lock_guard<mutex_type> lk2(_mutex);
        if ( this->global_stop_flag() )
          return ;
        --_bang_stat.wait_counter;
        ++_bang_stat.response_counter;
        ++_total_stat.response_counter;
          
        if ( !status) { ++_bang_stat.bad_check_counter; ++_total_stat.bad_check_counter; }
        if ( !good_gateway ) { ++_bang_stat.bad_gateway_counter; ++_total_stat.bad_gateway_counter; }
      });
    }
    return !this->global_stop_flag() ;
  }

  void log_stat(const bang_stat& cur_stat)
  {
    if ( cur_stat.request_counter != 0 || _bang_stat.wait_counter!=0)
    {
      TANK_LOG_MESSAGE(this->name() << " statistics")
      std::string interval;
      wjson::time_interval_mks<time_t>::serializer()( cur_stat.get_interval(), std::back_inserter(interval));
      if (cur_stat.request_counter != 0) { TANK_LOG_MESSAGE("\tTIME: " << interval) }
      if (cur_stat.request_counter != 0) { TANK_LOG_MESSAGE("\tRATE: " << cur_stat.get_rate() << " persec") }
      if (cur_stat.request_counter != 0) { TANK_LOG_MESSAGE("\tREQUESTS: " << cur_stat.request_counter) }
      if (cur_stat.response_counter != 0 || cur_stat.wait_counter!=0) 
          { TANK_LOG_MESSAGE("\tRESPONSES: " << cur_stat.response_counter) }
      if ( cur_stat.bad_check_counter!=0 ) { TANK_LOG_MESSAGE("\t\tBAD CHECK: " << cur_stat.bad_check_counter) }
      if ( cur_stat.bad_gateway_counter!=0 ) { TANK_LOG_MESSAGE("\t\tBAD GATEWAY: " << cur_stat.bad_gateway_counter) }
      if ( cur_stat.wait_counter!=0 ) { TANK_LOG_MESSAGE("\t\tWAIT: " << long(cur_stat.wait_counter) ) }
    }
    else
    {
      TANK_LOG_MESSAGE(this->name() << " statistics. No data.")      
    }
  }

  bool show_stat()
  {
    this->log_stat( this->start_stat() );

    return true; // for timer
  }

  bang_stat start_stat()
  {
    std::lock_guard<mutex_type> lk(_mutex);
    bang_stat res = _bang_stat;
    res.finish_time = std::chrono::high_resolution_clock::now();
    _bang_stat.start_time = std::chrono::high_resolution_clock::now();
    _bang_stat.request_counter = 0;
    _bang_stat.response_counter = 0;
    _bang_stat.bad_check_counter = 0;
    _bang_stat.bad_gateway_counter = 0;
    return res;
  }

private:
  std::shared_ptr<gun_interface> _gun;
  std::shared_ptr<I> _target;
  options_type _options;
  timer_id_t _timer = -1;
  size_t _gunfire_counter = 0;
  bang_stat _bang_stat;
  bang_stat _total_stat;
  mutex_type _mutex;
  time_meter _meter;
};

}
