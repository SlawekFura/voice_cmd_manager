#include <dbus/dbus.h>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "major_fsm.h"
#include "types.h"
#include "dbus_server.h"
#include "intent_manager.h"
#include "utils_fsm.h"

int main(int argc, char** argv)
{
    if (initialize_dbus_connection() != 0)
    {
        std::cout << "dbus conn is not set!" << std::endl;
        return -1;
    }

	common_fsm_backend main_fsm;
    main_fsm.start(); 
	while (true)
    {
        if (fetch_dbus_msg() == 0)
        {
            std::string content(extract_string_from_last_msg());
            const std::unique_ptr<EvtType>& intent_event = dispatch_intent(content);
            if (intent_event != nullptr)
            {
                fsm_process_event(main_fsm, *intent_event);
                common_fsm::pstate(main_fsm);
                // main_fsm.process_event(intent_event.get()); 
            }
            // go to Open, call on_exit on Empty, then action, then on_entry on Open
        }
        //usleep(100);
	}

    std::cout << "Finished processing" << std::endl;
    disconnect_dbus();
    return 0;
}
