#include <iostream>

#include "intent_manager.h"
#include "major_fsm.h"

std::unique_ptr<EvtType>& dispatch_intent(std::string msg_payload)
{
    if (intent_event_map.count(msg_payload) != 0)
    {
        return intent_event_map[msg_payload];
    }

    std::cout << "message not recognized: " << msg_payload << std::endl;
    return intent_event_map["invalid"];
}
