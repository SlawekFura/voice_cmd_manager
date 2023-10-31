#include <dbus/dbus.h>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "types.h"
#include "intent_manager.h"

static const char msg_interface[] = "test.signal.Type";
static const char msg_name[] = "Test";
DBusConnection* conn = NULL;
static DBusMessage* msg = NULL;

int initialize_dbus_connection()
{
    DBusError err;
    int ret;
    // initialise the errors
    dbus_error_init(&err);
    // connect to the bus
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err))
    {
        fprintf(stderr, "Dbus connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        return -1;
    }
    if (NULL == conn) {
        std::cout << "Null Connection to Dbus!" << std::endl;
        return -1;
    }
    // request a name on the bus
    ret = dbus_bus_request_name(conn, "test.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err))
    {
       fprintf(stderr, "Name Error (%s)\n", err.message);
       dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
    {
        std::cout << "DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER error!" << std::endl;
        ::dbus_connection_unref(conn);
        return -1;
    }

    // add a rule for which messages we want to see
    if (conn == nullptr)
    {
        std::cout << "Connection to Dbus not established!" << std::endl;
        return -1;
    }
	dbus_bus_add_match(conn, "type='signal',interface='test.signal.Type'",&err);
	dbus_connection_flush(conn);
	if (dbus_error_is_set(&err))
	{
	   fprintf(stderr, "Match Error (%s)\n", err.message);
	   return -1;
	}
    return 0;
}

int fetch_dbus_msg()
{
	// non blocking read of the next available message
	dbus_bool_t is_connected = dbus_connection_read_write(conn, 500);
    if (not is_connected)
    {
        return -1;
    }
	msg = dbus_connection_pop_message(conn);
    if (msg == NULL)
    {
        return -1;
    }

	// check if the message is a signal from the correct interface and with the correct name
	if (dbus_message_is_signal(msg, msg_interface, msg_name))
    {
        return 0;
	}
	else
    {
	  std::cout << "Message with bad interface or name!" << std::endl;
    }

    return -1;
}

std::string&& extract_string_from_last_msg()
{
	DBusMessageIter args;
    char* sigvalue;

	if (!dbus_message_iter_init(msg, &args))
	  	fprintf(stderr, "Message has no arguments!\n");
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    	fprintf(stderr, "Argument is not string!\n");
    else
    {
    	dbus_message_iter_get_basic(&args, &sigvalue);
        std::cout << "Received0: " << sigvalue << std::endl;
        return std::move(std::string(sigvalue));
    }
    return std::move(std::string());
}

void disconnect_dbus()
{
    dbus_connection_unref(conn);
}
