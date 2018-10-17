//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <string>

void wfc_exit();
void wfc_exit_with_error(const std::string& message);

void wfc_restart();
void wfc_restart_with_error(const std::string& message);

