#include <wfc/core/wfcglobal.hpp>
#include <wfc/core/icore.hpp>
#include <iow/memory.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace wfc{

std::shared_ptr<wfcglobal> wfcglobal::static_global;

wfcglobal::wfcglobal( io_context_type& ios)
  : io_context(ios)
{
  disable_statistics = true;
  stop_signal_flag = false;
}

void wfcglobal::clear()
{
  program_build_info = nullptr;
  wfc_build_info = nullptr;
  args.clear();
  idle.clear();
  after_start.clear();
  before_stop.clear();
  after_stop.clear();
  nocall_handler = nullptr;
  doublecall_handler = nullptr;
  cpu.clear();
  registry.clear();
  common_workflow = nullptr;
}

wfcglobal::~wfcglobal()
{
  this->clear();
}

std::string wfcglobal::find_config(const std::string& filename) const
{
  using namespace boost::filesystem;
  auto curpath = system_complete(filename).lexically_normal();
  if ( exists(curpath) )
    return curpath.native();

  if ( !this->config_path.empty() )
  {
    curpath = this->config_path;
    curpath /= filename;
    if ( exists(curpath) )
      return curpath.lexically_normal().native();
  }

  return this->find_working(filename);
}

std::string wfcglobal::find_working(const std::string& filename) const
{
  using namespace boost::filesystem;
  auto curpath = system_complete(filename).lexically_normal();

  if ( exists(curpath) )
    return curpath.native();

  if ( !this->working_directory.empty() )
  {
    curpath = this->working_directory;
    curpath /= filename;
    if ( exists(curpath) )
      return curpath.lexically_normal().native();
  }

  if ( !this->program_path.empty() )
  {
    curpath = this->program_path;
    curpath /= filename;
    if ( exists(curpath) )
      return curpath.lexically_normal().native();
  }

  return std::string();
}

std::string wfcglobal::make_directory(const std::string& pathname, std::string* err ) const
{
  using namespace boost;
  using namespace boost::filesystem;
  auto curpath = system_complete(pathname).lexically_normal();

  if ( exists(curpath) )
    return curpath.native();

  if ( !curpath.is_absolute() )
  {
    curpath = this->working_directory;
    curpath /= pathname;

    if ( exists(curpath) )
      return curpath.native();
  }

  system::error_code ec;
  if ( create_directories(curpath, ec) )
    return curpath.lexically_normal().native();

  if ( err != nullptr )
  {
    *err = ec.message();
  }
  return std::string();
}

std::string wfcglobal::make_directory_for_file(const std::string& pathname, std::string* err) const
{
  using namespace boost;
  using namespace boost::filesystem;
  path curpath(pathname);
  std::string err1;
  std::string filename = curpath.filename().native();
  curpath = make_directory( curpath.parent_path().native(), &err1 );
  if ( !err1.empty() )
  {
    if ( err!=nullptr )
      *err = err1;
    return std::string();
  }
  curpath/=filename;
  return curpath.lexically_normal().native();
}


}

