#include <dbus/dbus.h>
#include <stdio.h>

void send_message(const char* message) {
    DBusMessage* msg;
    DBusMessageIter args;
    DBusConnection* conn;
    DBusError err;
    DBusPendingCall* pending;
    DBusMessage* reply;
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

    // Create a new method call and check for errors
    msg = dbus_message_new_method_call("test.method.server", // target for the method call
                                       "/test/method/Object", // object to call on
                                       "test.method.Type", // interface to call on
                                       "Method"); // method name
    if (NULL == msg) {
        fprintf(stderr, "Message Null\n");
        exit(1);
    }

    // Append arguments
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &message)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }

    // Send the message and get a handle for a reply
    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    if (NULL == pending) {
        fprintf(stderr, "Pending Call Null\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    // Free the message
    dbus_message_unref(msg);

    // Block until we receive a reply
    dbus_pending_call_block(pending);

    // Get the reply message
    reply = dbus_pending_call_steal_reply(pending);
    if (NULL == reply) {
        fprintf(stderr, "Reply Null\n");
        exit(1);
    }
    // Free the pending message handle
    dbus_pending_call_unref(pending);

    // Read the reply arguments
    if (!dbus_message_iter_init(reply, &args))
        fprintf(stderr, "Reply has no arguments!\n");
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        fprintf(stderr, "Reply argument is not string!\n");
    else {
        char* reply_msg = NULL;
        dbus_message_iter_get_basic(&args, &reply_msg);
        printf("Reply received: %s\n", reply_msg);
    }

    // Free the reply
    dbus_message_unref(reply);
}

int main() {
    send_message("Hello, D-Bus!");
    return 0;
}

