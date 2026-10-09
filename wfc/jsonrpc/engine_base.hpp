//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2018
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/domain_object.hpp>
#include <wfc/iinterface.hpp>
#include <wfc/memory.hpp>
#include <wjrpc/outgoing/outgoing_holder.hpp>
#include <wfc/jsonrpc/statistics_options.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace wfc{ namespace jsonrpc{

template<typename Interface, typename EngineType, typename OptionsType >
class engine_base
  : public domain_object< Interface, OptionsType, statistics_options>
{
public:
  typedef engine_base<Interface, EngineType, OptionsType> self;
  typedef domain_object< Interface, OptionsType, statistics_options> super;

  typedef Interface interface_type;
  typedef EngineType engine_type;
  typedef OptionsType options_type;

  typedef std::shared_ptr<engine_type> engine_ptr;

  typedef typename options_type::engine_options engine_options;

  typedef typename super::io_id_t io_id_t;
  typedef typename super::data_ptr data_ptr;
  typedef typename super::data_type data_type;
  typedef typename super::output_handler_t output_handler_t;
  typedef wflow::workflow workflow_type;
  typedef workflow_type::timer_id_t timer_id_t;

public:

  virtual ~engine_base()
  {
    _engine=nullptr;
  }

  virtual void stop() final override
  {
    if ( _engine != nullptr )
    {
      if ( auto wf = this->get_workflow() )
      {
        wf->release_timer(_timer_id);
        wf->release_timer(_stat_timer_id);
      }
      _engine->stop();

    }
    _engine = nullptr;
  }


  virtual void reg_io(io_id_t io_id, std::weak_ptr<iinterface> witf) override
  {
    if ( this->suspended() )
      return;
    if ( _engine != nullptr )
    {
      _engine->reg_io( io_id, this->wrap([witf]( data_ptr d, io_id_t io_id1, output_handler_t handler)
      {
        if (auto pitf = witf.lock() )
        {
          pitf->perform_io( std::move(d), io_id1, handler);
        }
      }, nullptr));
    }
  }

  virtual void unreg_io(io_id_t io_id) override
  {
    if ( this->suspended() )
      return;
    if ( _engine != nullptr )
    {
      _engine->unreg_io(io_id);
    }
  }

  virtual void perform_io(data_ptr d, io_id_t io_id, output_handler_t handler) override
  {
    if ( this->suspended() )
    {
      if (handler!=nullptr)
        handler(std::move(d) );
      return;
    }

    if ( _engine == nullptr )
      return;

    _engine->perform_io( std::move(d), io_id, std::move(handler) );
  }

  engine_ptr engine() const { return _engine; }

  typedef std::map<std::string, std::vector<std::string> > schema_map_t;

  static std::list<std::string> get_method_list(schema_map_t& schema_map)
  {
    std::list<std::string> result;
    typedef typename engine_type::handler_type::aspect::template select_group< wjrpc::_method_ >::type tag_list;
    self::get_method_list_(result, schema_map, tag_list() );
    return result;
  }

  static std::string schema_help(const std::string& component_name, const std::string& args, const std::string& base_help )
  {
    typedef typename engine_type::handler_type handler_type;
    std::stringstream ss;
    schema_map_t schema_map;
    std::list<std::string> method_list = get_method_list(schema_map);
    if ( args.empty() )
    {
      ss << base_help << std::endl;
      std::string method_ws_schema;
      ss << "Allowed methods for '" << component_name << "' [allowed schemas]:" << std::endl;
      for (auto n : method_list)
      {
        ss << "  " << n;
        auto method_schemas = schema_map[n];
        if ( !method_schemas.empty() )
        {
          std::stringstream ss1;
          bool has_schema_help = !method_ws_schema.empty();
          if ( !has_schema_help )
          {
            ss1 << "\t--help " << component_name << ":schema:"   << n << "=";
          }

          ss << " [";
          for (size_t i = 0; i < method_schemas.size() ; ++i)
          {
            ss << method_schemas[i];
            ss1 << method_schemas[i];
            if ( i == method_schemas.size() - 1)
            {
              ss << "]";
            }
            else
            {
              ss << ",";
              ss1 << ",";
            }
          }
          method_ws_schema = ss1.str();
        }
        ss << std::endl;
      }

      if ( !method_list.empty() )
      {
        ss << "Get the JSON-RPC default schema:"<< std::endl;
        ss << "\t--help " << component_name << ":schema"    << std::endl;
        ss << "\tExamples for method "<< method_list.front() << ":" << std::endl;
        ss << "\t--help " << component_name << ":schema:"   << method_list.front() << std::endl;
        ss << "\t--help " << component_name << ":params:"   << method_list.front() << std::endl;
        ss << "\t--help " << component_name << ":result:"   << method_list.front() << std::endl;
        ss << "\t--help " << component_name << ":request:"  << method_list.front() << std::endl;
        ss << "\t--help " << component_name << ":response:" << method_list.front();

        if ( !method_ws_schema.empty() )
        {
          ss << std::endl;
          ss << "All allowed schemas: " << method_ws_schema << std::endl;
          ss << "Get the JSON-RPC named schema example:"<< std::endl;
          ss << "\t--help " << component_name << ":schema:"   << method_list.front() << "=" << method_ws_schema;
        }
      }
    }
    else
    {
      typedef typename handler_type::schema_list_t schema_list_t;
      typedef typename schema_list_t::value_type schema_type;
      typedef typename schema_type::json schema_json;
      typedef typename schema_json::list schema_list_json;

      size_t eqpos=args.find('=');
      std::string argname = args.substr(0, eqpos);
      std::string argval = ((eqpos!=std::string::npos) ? args.substr(eqpos+1) : std::string());
      std::vector<std::string> schema_list;
      boost::split( schema_list, argval, boost::is_any_of(",") );
      size_t schpos = argname.find(':');
      std::string schema_tname = argname.substr(0, schpos);
      std::string method_name = schpos!=std::string::npos ? argname.substr(schpos+1) : std::string();

      schema_list_t schema = handler_type::create_schema(schema_list);
      if ( schema_tname=="schema" && method_name.empty() )
      {
        typename schema_list_json::serializer()(schema, std::ostreambuf_iterator<char>(ss) );
      }
      else
      {
        auto itr = std::find_if(schema.begin(), schema.end(),
          [method_name](const schema_type& sch)->bool{
            return sch.name == method_name;
          }
        );

        if ( itr!=schema.end() )
        {
          using namespace wjson::literals;
          if ( schema_tname=="schema" )
            typename schema_json::serializer()(*itr, std::ostreambuf_iterator<char>(ss) );
          else if ( schema_tname=="params" )
            ss << itr->params;
          else if ( schema_tname=="result" )
            ss << itr->result;
          else if ( schema_tname=="request" )
            ss << "{'jsonrpc':'2.0','method':'"_json << method_name << "','params':"_json << itr->params << ",'id':1}"_json;
          else if ( schema_tname=="response" )
            ss << "{'jsonrpc':'2.0','result':"_json << itr->result << ",'id':1}"_json;
          else
            ss << "Error: Invalid argument '" << schema_tname << "'";
        }
        else
          ss << "ERROR: procedure '"<<  method_name<<"' not found";
      }
    }
    return ss.str();
  }
protected:

  static void get_method_list_(std::list<std::string>& , schema_map_t&, fas::empty_list)
  {
  }

  template<typename L>
  static void get_method_list_(std::list<std::string>& res, schema_map_t& schema_map, L)
  {
    typedef typename fas::head<L>::type tag;
    res.push_back( tag()() );
    typedef typename engine_type::handler_type::aspect::template advice_cast< tag >::type method_t;
    method_t::get_schema_list(schema_map[tag()()]);
    self::get_method_list_( res, schema_map, typename fas::tail<L>::type() );
  }


  void initialize_engine(options_type eopt)
  {
    eopt.domain_name = this->name();
    eopt.powner = &(this->owner());
    eopt.tracking_id = this->get_id();
    eopt.disable_tracking_log = false;
    
    if ( _engine == nullptr )
    {
      _engine = std::make_shared<engine_type>();
      _engine->start(eopt, ::iow::io::create_id<size_t>() );
    }
    else
    {
      _engine->reconfigure( eopt );
    }

    if ( auto wf = this->get_workflow() )
    {
      wf->release_timer( _timer_id );
      if ( eopt.remove_outdated_ms != 0 )
      {
        auto pengine = this->_engine;
        _timer_id = wf->create_timer(
          std::chrono::milliseconds(eopt.remove_outdated_ms),
          [pengine]() -> bool
          {
            JSONRPC_LOG_DEBUG( " JSON-RPC remove outdated...");
            if ( size_t count = pengine->remove_outdated() )
            {
              JSONRPC_LOG_WARNING( "JSON-RPC: " << count << " calls is outdated.");
            }
            return true;
          }
        );
      }
    }
    this->initialize_statistics_();
  }

private:
  void initialize_statistics_()
  {
    if ( auto stat = this->get_statistics() )
    {
      auto sopt = this->statistics_options();
      if ( auto wf = this->get_workflow() )
      {
        wf->release_timer(_stat_timer_id);
        if ( sopt.interval_ms != 0 )
        {
          auto sname = this->name();
          auto handler_map  = stat->create_value_meter(sname + sopt.handler_map);
          auto result_map   = stat->create_value_meter(sname + sopt.result_map);
          auto result_queue = stat->create_value_meter(sname + sopt.result_queue);
          _stat_timer_id = wf->create_timer(
            std::chrono::milliseconds(sopt.interval_ms),
            [handler_map, result_map, result_queue, this]()->bool
            {
              if ( nullptr != this->get_statistics() )
              {
                auto si = this->_engine->sizes();
                handler_map.create( static_cast<wrtstat::value_type>(si.handler_map), static_cast<wrtstat::size_type>(0));
                result_map.create(  static_cast<wrtstat::value_type>(si.result_map), static_cast<wrtstat::size_type>(0));
                result_queue.create(static_cast<wrtstat::value_type>(si.result_queue), static_cast<wrtstat::size_type>(0));
                return true;
              }
              return false;
            }
          );
        }
      }
    }
  }


private:
  timer_id_t _timer_id = 0;
  timer_id_t _stat_timer_id = 0;
protected:
  engine_ptr _engine;
};

}}
