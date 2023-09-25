#include "intent_manager.h"

Intent match_intent(std::string msg_payload)
{
    if (intent_dbus_map.count(msg_payload) != 0)
    {
        return intent_dbus_map[msg_payload];
    }
    return Intent::Intent_invalid;
}
