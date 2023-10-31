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
#include "audio_mgr.h"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <portaudio.h>

#include <syslog.h>


void sigint_handler(int) {
    std::cout << "SigINT handling!" << std::endl;
    disconnect_dbus();
    Pa_Terminate();
    exit(0);
}

Audio_Manager::UserCbkData Audio_Manager::user_data = {};

int main(int argc, char** argv)
{
    // TODO: add syslogging
    // openlog("Krysia_service", LOG_PID, LOG_USER);
    // syslog(LOG_INFO, "Start logging");
    // closelog();
    // return 0;
    std::cout << "main -1" << std::endl;

    signal(SIGINT, sigint_handler);

    std::cout << "main 0" << std::endl;

    if (initialize_dbus_connection() != 0)
    {
        std::cout << "dbus conn is not set!" << std::endl;
        return -1;
    }

    std::cout << "main 1" << std::endl;
    // Audio_Manager audio_manager;
    std::cout << "main 2" << std::endl;

    //while(1){}
	common_fsm_backend main_fsm;
    main_fsm.start();
	while (true)
    {
        if (fetch_dbus_msg() == 0)
        {
            std::string content(extract_string_from_last_msg());
            const std::unique_ptr<EvtType>& intent_event = dispatch_intent(content);
            if (intent_event != intent_event_map["invalid"])
            {
                fsm_process_event(main_fsm, *intent_event);
                common_fsm::pstate(main_fsm);
            }
        }
	}

    std::cout << "Finished processing" << std::endl;
    disconnect_dbus();
    Pa_Terminate();
    return 0;
}
