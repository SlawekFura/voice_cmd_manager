#include <dbus/dbus.h>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "types.h"
#include "dbus_server.h"
#include "intent_manager.h"

int main(char* argc, char** argv)
{
	DBusMessage* msg;
	DBusMessageIter args;
    DBusPendingCall* pending;
    char sigvalue;
    DBusConnection* conn;

    Intent intent = Intent::Intent_invalid;
    if (initialize_dbus_connection(&conn) != 0)
    {
        return -1;
    }
    if (conn == nullptr)
    {

        std::cout << "conn is nullptr!" << std::endl;
        return -1;
    }
    else
        std::cout << "conn is not nullptr" << std::endl;

	while (true)
    {
        if (fetch_dbus_msg(conn, msg) == 0)
        {
            intent = retrieve_intent_from_dbus_msg(msg);
        }
        usleep(100);
	}

    std::cout << "Finished processing" << std::endl;
    disconnect_dbus(conn);
    return 0;
}
