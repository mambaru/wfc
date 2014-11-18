
class module_impl;
  
class module: public ::wfc::imodule
{
public:
  module();

  virtual priority startup_priority() const;
  
  virtual priority shutdown_priority() const;
  
  virtual std::string version() const;
  
  virtual std::string description() const;
  
  virtual std::string generate(const std::string& type) const;

  virtual bool parse_config(const std::string& confstr);
  
  virtual void create( const std::string& name, std::shared_ptr< ::wfc::global> g );
  
  virtual void configure(const std::string& conf, const std::string& arg);
  
  virtual void initialize(const std::string& arg);
  
  virtual void start(const std::string& arg);
  
  virtual void stop(const std::string& arg);
  
  virtual void shutdown(const std::string& arg);

private:

  std::shared_ptr<module_impl> _impl;

};
