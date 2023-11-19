#pragma once

#include "types.h"

int initialize_dbus_connection();
int fetch_dbus_msg();
void disconnect_dbus();
std::string extract_string_from_last_msg();
