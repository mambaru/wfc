#pragma once

#include <wfc/module/instance_options_json.hpp>

#include <wfc/module/iobject.hpp>
#include <wfc/core/global.hpp>

#include <fas/typemanip.hpp>
#include <memory>
#include <string>
#include <stdexcept>

namespace wfc{

template< 
  typename Name,
  typename Instance,
  typename DomainJson,
  bool Singleton = true
>
class basic_object
  : public iobject
{
public:
  enum { is_singleton = Singleton};

  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef Instance    instance_type;
  typedef typename instance_type::options_type options_type;
  typedef typename instance_type::interface_type interface_type;
  typedef instance_options_json<DomainJson, Singleton> instance_json;
  typedef std::shared_ptr<instance_type> instance_ptr;
  typedef std::map< std::string, instance_ptr> instance_map;
  
  virtual ~basic_object()
  {
    if ( _global )
    {
      _global->registry.erase( "object", this->name() );
    }
  }
  typedef Name object_name;

  virtual std::string name() const 
  {
    return object_name()();
  }

  virtual std::string description() const
  {
    return "no description";
  }

  virtual std::string interface_name() const 
  {
    return typeid( interface_type ).name();
  }

  virtual std::string generate(const std::string& type) const 
  {
    options_type inst_opt;
    inst_opt.name = this->name() + "1";
    this->generate_options(inst_opt, type);
    return this->serialize_(inst_opt, fas::bool_<Singleton>() );
  }

  virtual bool parse(const std::string& stropt)
  {
    //options_type opt;
    this->unserialize_( stropt, fas::bool_<Singleton>());
    return true;
    //return this->parse1_(opt, stropt);
  }

  /*
  bool parse1_(options_type& opt, const std::string& stropt)
  {
    auto beg = stropt.begin();
    auto end = stropt.end();
    
    try
    {
      beg = ::iow::json::parser::parse_space(beg, end);
      typename instance_json::serializer serializer;
      serializer(opt, beg, end);
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Invalid json configuration sington module '"<< this->name() << "':" << std::endl;
      ss << e.message( beg, end );
      throw std::domain_error(ss.str());
    }
    return true;
  }
  */
  
  virtual void create( std::shared_ptr<wfcglobal> g) 
  {
    _global = g;
    this->create_(fas::bool_<Singleton>());
  }
  
  void create_(fas::true_)
  {
    std::cout << "DEBUG: SINGLETON OBJECT " << std::endl;
    auto inst = std::make_shared<instance_type>();
    _instances[ this->name() ] = inst;
    options_type opt;
    opt.name = this->name();
    opt.enabled = true;
    _global->registry.set("instance", this->name(), inst);
    inst->create(_global);
    inst->configure(opt);
    inst->initialize();
  }

  void create_(fas::false_) { }

  std::string parse_(const std::string& stropt)
  {
    auto beg = stropt.begin();
    auto end = stropt.end();
    
    try
    {
      beg = ::iow::json::parser::parse_space(beg, end);
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Invalid parse json configuration '"<< this->name() << "':" << std::endl;
      ss << e.message( beg, end );
      throw std::domain_error(ss.str());
    }
    return std::string(beg, end);
  }

  template<typename JSON>
  std::string serialize_( const typename JSON::target& value ) const
  {
    std::string result;
    typename JSON::serializer serializer;
    serializer( value, std::back_inserter(result) );
    return result;
  }

  std::string  serialize_( const options_type& opt, fas::true_ ) const
  {
    return serialize_<instance_json>(opt);
  }

  std::string  serialize_( const options_type& opt, fas::false_ ) const
  {
    typedef std::vector<options_type> vect_opt;
    typedef ::iow::json::array< std::vector< instance_json > > list_json;
    vect_opt vopt = {opt};
    return this->serialize_<list_json>(vopt);
  }

  template<typename JSON>
  void unserialize_( typename JSON::target& value, const std::string&  str )
  {
    try
    {
      typename JSON::serializer serializer;
      serializer( value, str.begin(), str.end() );
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Json unserialize error for object '"<< this->name() << "':" << std::endl;
      ss << e.message( str.begin(), str.end() );
      throw std::domain_error(ss.str());
    }
  }
  
  
  
  std::vector< options_type > 
  unserialize_( const std::string& stropt, fas::true_ )
  {
    options_type opt;
    this->unserialize_<instance_json>(opt, stropt);
    opt.name = Name()();
    return std::vector< options_type >( {opt} );
  }
  
  std::vector< options_type > 
  unserialize_( const std::string& stropt, fas::false_ )
  {
    typedef std::vector< options_type > options_list;
    typedef ::iow::json::array< std::vector< instance_json > > list_json;
    options_list opt;
    this->unserialize_<list_json>(opt, stropt);
    return opt;
  }


  virtual void configure(const std::string& stropt, const std::string& /*arg*/)
  {
    std::set<std::string> stop_list;
    for (const auto& item : _instances )
      stop_list.insert(item.first);
    
    auto optlist =  unserialize_( stropt, fas::bool_<is_singleton>() );
    for (const auto& opt : optlist )
    {
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, inst) ).first;
        if ( _global )
        {
          _global->registry.set("instance", opt.name, inst);
        }
        inst->create( _global );
      }
      itr->second->configure(opt);
      stop_list.erase(opt.name);
    }
    
    for ( const auto& name: stop_list )
    {
      auto itr = _instances.find(name);
      if ( itr == _instances.end() )
      {
        if ( _global )
        {
          _global->registry.erase("instance", name);
        }
        itr->second->stop("");
        _instances.erase(itr);
      }
    }
    
    /*
    std::vector<std::string> raw_list = this->raw_unserialize_(stropt, fas::bool_<is_singleton>() );
    for (const auto& raw : raw_list )
    {
      instance_options_type opt = this->unserialize_instance_(raw, fas::bool_<is_singleton>());
      auto itr = _instances.find(opt.name);
      if ( itr == _instances.end() )
      {
        auto inst =  std::make_shared<instance_type>();
        itr = _instances.insert( std::make_pair( opt.name, std::make_pair(inst, inst)) );
      }
    }
    */
    
    /*
    options_type opt;
    this->parse_(opt, stropt);
    auto optlist = helper::object2list(opt);
    for ( const auto& op : optlist )
    {
      // Всякие проверки 
      auto inst =  std::make_shared<instance_type>();
      //_instances[op.name] = inst;
      
      if ( _global )
      {
        // Регистрация (перенести в instance)
        //_global->registry.set( "instance", op.name, inst);
      }
    }
    */
  }

// only for external control
  virtual void start(const std::string& arg) 
  {
    for ( auto& p : _instances )
    {
      p.second->start(arg);
    }
  }

  virtual void stop(const std::string& arg) 
  {
    for ( auto& p : _instances )
    {
      p.second->stop(arg);
    }
    
  }

  virtual void generate_options(options_type& opt, const std::string& type) const 
  {
    /*auto tmp = std::make_shared<instance_type>();
    tmp->generate( opt, type);
    */
    instance_type().generate( opt, type);
  }

private:
  global_ptr   _global;
  instance_map _instances;
};

template< typename Name, typename Instance, typename DomainJson>
class singleton: public basic_object<Name, Instance, DomainJson, true> {};

template< typename Name, typename Instance, typename DomainJson>
class multiton: public basic_object<Name, Instance, DomainJson, false> {};



}
