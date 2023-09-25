#include "types.h"

std::map<std::string, Intent> intent_dbus_map =
{
    {"example", Intent::Intent_invalid},
    {"hello", Intent::Intent_greetings},
    {"goodbye", Intent::Intent_goodbye}
};

