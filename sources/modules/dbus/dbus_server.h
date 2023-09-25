#pragma once

#include "types.h"

int initialize_dbus_connection(DBusConnection**);
int fetch_dbus_msg(DBusConnection*, DBusMessage*);
void disconnect_dbus(DBusConnection*);
std::string extract_string_from_msg(DBusMessage*);
