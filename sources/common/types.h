#pragma once

#include <map>

enum class Intent
{
    Intent_invalid,
    Intent_greetings,
    Intent_goodbye,
};

extern std::map<std::string, Intent> intent_dbus_map;


