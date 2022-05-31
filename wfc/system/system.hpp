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
#include <unistd.h>

namespace wfc{

int is_atty(int fd);

bool is_atty_stdout();

bool is_atty_stderr();

void sleep( int millisec );

std::function<void()> daemonize(bool wait = false);

bool change_user(const std::string& username, std::string* err);

bool change_working_directory(const std::string& working_directory, std::string* err);

bool autoup(
  time_t timeout,
  bool success_autoup = false,
  std::function<bool(pid_t pid, int, int, time_t)> before = nullptr,
  std::function<void(int, int, time_t)> after = nullptr
);

int dumpable(bool enable);

}
