//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2007, 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <functional>
#include <stdexcept>
#include <ctime>
#include <string>

namespace wfc{

int error_code();

std::string strerror(int lasterror);

struct system_error
  : public std::runtime_error
{
  explicit system_error(const std::string& msg);
};


int is_atty(int fd);

bool is_atty_stdout();

bool is_atty_stderr();

void sleep( int millisec );

std::function<void()> daemonize(bool wait = false);

bool change_user(std::string username);

bool change_working_directory(std::string working_directory);

void autoup(time_t timeout, std::function<bool(bool, int, time_t)> f = nullptr);

int dumpable();

}
