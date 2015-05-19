#pragma once

#include <wfc/module/instance_options_json.hpp>

#include <wfc/core/iobject.hpp>
#include <wfc/core/global.hpp>

#include <fas/typemanip.hpp>
#include <memory>
#include <string>
#include <stdexcept>


namespace wfc{
  

template<typename Name, typename Instance, typename DomainJson, bool Singleton >
struct object_helper;

template<typename Name, typename Instance, typename DomainJson>
struct object_helper<Name, Instance, DomainJson, true>
  
{
  typedef Instance    instance_type;
  typedef DomainJson  domain_json;
  typedef typename instance_type::options_type item_options_type;
  typedef typename instance_type::object_type::options_type domain_options_type;
  
  typedef basic_instance_options_json< 
            domain_options_type, 
            domain_json
          > item_options_json;
          
  typedef item_options_json options_json;
  typedef item_options_type options_type;
  
  
  static options_type instance2object(options_type opt)
  {
    opt.name = Name()();
    return opt;
  }

  static std::vector<options_type> object2list(options_type opt)
  {
    opt.name = Name()();
    return std::vector<options_type>({opt});
  }
};


template<typename Name, typename Instance, typename DomainJson>
struct object_helper<Name, Instance, DomainJson, false>
{
  typedef Instance    instance_type;
  typedef DomainJson  domain_json;
  typedef typename instance_type::options_type item_options_type;
  typedef typename instance_type::object_type::options_type domain_options_type;
  typedef instance_options_json< 
            domain_options_type, 
            domain_json
          > item_options_json;


  typedef std::vector<item_options_type> options_type;
  typedef ::iow::json::array< std::vector< item_options_json > > options_json;
  
  static options_type instance2object(const item_options_type& opt)
  {
    auto res = opt;
    if ( res.name.empty() )
    {
      res.name = std::string(Name()())+"1";
    }
    return options_type({res});
  }

  static options_type object2list(const options_type& opt)
  {
    return opt;
  }
};


  // base for singletion or multiton
// для sington массив из одного объекта
// multiton частый случай singletion
  
template< 
  typename Name, 
  typename Instance, 
  typename DomainJson,
  bool Singleton = true
>
class basic_object
  : public iobject
  , public std::enable_shared_from_this<iobject>
{
  // Автоопределялка для Obj: object_base или object_impl
public:
  enum { is_singleton = Singleton};
  typedef object_helper<Name, Instance, DomainJson, Singleton> helper;
  friend struct object_helper<Name, Instance, DomainJson, Singleton>;
  
  typedef typename helper::options_type options_type;
  typedef typename helper::options_json options_json;

  typedef typename helper::item_options_type instance_options_type;
  typedef typename helper::item_options_json instance_options_json;

  typedef std::shared_ptr<wfcglobal> global_ptr;
  
  typedef Instance    instance_type;
  typedef std::shared_ptr<instance_type> instance_ptr;
  // typedef std::pair<instance_ptr, std::string> instance_pair;
  typedef std::map< std::string, instance_ptr> instance_map;
  
  virtual ~basic_object()
  {
    if ( _global )
    {
      _global->registry.erase( "object", this->name() );
    }
  }
  
  typedef Name object_name;
  /*
  typedef Instance    instance_type;
  typedef DomainJson  domain_json;
  typedef typename instance_type::options_type options_type;
  typedef std::shared_ptr<wfcglobal> global_ptr;
  typedef std::shared_ptr<instance_type> instance_ptr;
  typedef instance_options_json< options_type, domain_json> instance_json;
  */

  virtual std::string name() const 
  {
    return Name()();
  }

  virtual std::string description() const
  {
    return "no description";
  }

  virtual std::string generate(const std::string& type) const 
  {
    instance_options_type inst_opt;
    this->generate_options(inst_opt, type);
    auto opt = helper::instance2object(inst_opt);
    typename options_json::serializer serializer;
    std::string result;
    serializer( opt, std::back_inserter(result) );
    return result;
  }

  virtual void parse(const std::string& stropt)
  {
    options_type opt;
    this->parse1_(opt, stropt);
  }

  void parse1_(options_type& opt, const std::string& stropt)
  {
    auto beg = stropt.begin();
    auto end = stropt.end();
    
    try
    {
      beg = ::iow::json::parser::parse_space(beg, end);
      typename options_json::serializer serializer;
      serializer(opt, beg, end);
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Invalid json configuration sington module '"<< this->name() << "':" << std::endl;
      ss << e.message( beg, end );
      throw std::domain_error(ss.str());
    }
  }
  
  virtual void create( std::shared_ptr<wfcglobal> g) 
  {
    _global = g;
    if ( _global )
    {
      // TODO: module
      _global->registry.set( "object", this->name(), this->shared_from_this() );
    }
  }
  
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
  
  /*
  std::vector<std::string> raw_unserialize_(const std::string& stropt, fas::true_ )
  {
    std::string stropt1 = this->parse_(stropt);
    return std::vector<std::string>(stropt1);
  }

  std::vector<std::string> raw_unserialize_(const std::string& stropt, fas::false_ )
  {
    //std::string stropt = this->parse_(stropt);
    typedef ::iow::json::array< std::vector< ::iow::json::raw_value<std::string> > > raw_array;
    std::vector<std::string> result;
    raw_array::serializer()( result, stropt.begin(),  stropt.end() );
    return std::vector<std::string>(stropt);
  }
  
  instance_options_type unserialize_instance_( const std::string& stropt, fas::false_ )
  {
    instance_options_type opt;
    try
    {
      instance_options_json::serializer( opt, stropt.begin(), stropt.end() );
    }
    catch(const ::iow::json::json_error& e)
    {
      std::stringstream ss;
      ss << "Instance unserialize json error for object '"<< this->name() << "':" << std::endl;
      ss << e.message( stropt.begin(), stropt.end() );
      throw std::domain_error(ss.str());
    }
  }
  
  instance_options_type unserialize_instance_( const std::string& stropt, fas::true_ )
  {
    instance_options_type opt = unserialize_instance_( stropt, fas::false_);
    opt.name = Name()();
    return opt;
  }
  */
  
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
  
  
  
  std::vector< instance_options_type > unserialize_( const std::string& stropt, fas::true_ )
  {
    instance_options_type opt;
    this->unserialize_<instance_options_json>(opt, stropt);
    opt.name = Name()();
    return std::vector< instance_options_type >( {opt} );
  }
  
  std::vector< instance_options_type > unserialize_( const std::string& stropt, fas::false_ )
  {
    typedef std::vector< instance_options_type > options_list;
    typedef ::iow::json::array< std::vector< instance_options_json > > list_json;
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

  virtual void generate_options(instance_options_type& opt, const std::string& type) const 
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
