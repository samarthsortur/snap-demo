#include <dbus/dbus.h>
#include <stdio.h>

void listen_for_messages() {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusMessage* reply;
    DBusConnection* conn;
    DBusError err;
    int ret;

    // Initialize the errors
    dbus_error_init(&err);

    // Connect to the system bus and check for errors
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (NULL == conn) {
        exit(1);
    }

    // Request our name on the bus and check for errors
    ret = dbus_bus_request_name(conn, "test.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        exit(1);
    }

    // Loop, waiting for messages
    while (1) {
        // Non blocking read of the next available message
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        // Loop again if we haven't read a message
        if (NULL == msg) {
            usleep(10000);
            continue;
        }

        // Check this is a method call for the right interface & method
        if (dbus_message_is_method_call(msg, "test.method.Type", "Method")) {
            // Read the arguments
            if (!dbus_message_iter_init(msg, &args))
                fprintf(stderr, "Message has no arguments!\n");
            else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
                fprintf(stderr, "Argument is not string!\n");
            else {
                char* param = NULL;
                dbus_message_iter_get_basic(&args, &param);
                printf("Received: %s\n", param);

                // Create a reply from the received message
                reply = dbus_message_new_method_return(msg);
                if (NULL == reply) {
                    fprintf(stderr, "Out Of Memory!\n");
                    exit(1);
                }

                // Append arguments to the reply
                DBusMessageIter reply_args;
                dbus_message_iter_init_append(reply, &reply_args);
                const char* reply_msg = "Message received!";
                if (!dbus_message_iter_append_basic(&reply_args, DBUS_TYPE_STRING, &reply_msg)) {
                    fprintf(stderr, "Out Of Memory!\n");
                    exit(1);
                }

                // Send the reply and flush the connection
                if (!dbus_connection_send(conn, reply, NULL)) {
                    fprintf(stderr, "Out Of Memory!\n");
                    exit(1);
                }
                dbus_connection_flush(conn);
                printf("Reply sent: %s\n", reply_msg);

                // Free the reply
                dbus_message_unref(reply);
            }
        }

        // Free the message
        dbus_message_unref(msg);
    }
}

int main() {
    listen_for_messages();
    return 0;
}

