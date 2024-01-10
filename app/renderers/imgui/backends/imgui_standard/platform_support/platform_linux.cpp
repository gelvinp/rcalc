#ifdef ENABLE_PLATFORM_LINUX

#include "app/renderers/imgui/backends/imgui_standard/imgui_standard_backend.h"
#include "core/logger.h"

#ifdef DBUS_ENABLED

#include <dbus/dbus.h>

RCalc::Result<bool> RCalc::ImGuiStandardBackend::is_dark_theme() const {
    DBusError error;
    dbus_error_init(&error);

    DBusConnection* p_conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (dbus_error_is_set(&error)) {
        Logger::log_info("Failed to open D-Bus connection!\n\t%s", error.message);
        dbus_error_free(&error);
        return Err(ERR_NOT_SUPPORTED, "Failed to open D-Bus connection.");
    }

    DBusMessage* p_msg = dbus_message_new_method_call(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Settings",
        "Read"
    );
    
    const char* zs_namespace = "org.freedesktop.appearance";
    const char* zs_key = "color-scheme";

    dbus_message_append_args(
        p_msg,
        DBUS_TYPE_STRING, &zs_namespace,
        DBUS_TYPE_STRING, &zs_key,
        DBUS_TYPE_INVALID
    );

    DBusMessage* p_resp = dbus_connection_send_with_reply_and_block(p_conn, p_msg, 50, &error);
    if (dbus_error_is_set(&error)) {
        Logger::log_info("Failed to read appearance from D-Bus!\n\t%s", error.message);
        dbus_error_free(&error);
        dbus_connection_unref(p_conn);
        return Err(ERR_NOT_SUPPORTED, "Failed to read appearance from D-Bus.");
    }

    DBusMessageIter msg_iter[3];
    for (int index = 0; index < 3; ++index) {
        if (index == 0) {
            dbus_message_iter_init(p_resp, &msg_iter[0]);
        }
        else {
            dbus_message_iter_recurse(&msg_iter[index - 1], &msg_iter[index]);
        }

        int desired_type = (index == 2) ? DBUS_TYPE_UINT32 : DBUS_TYPE_VARIANT;
        if (dbus_message_iter_get_arg_type(&msg_iter[index]) != desired_type) {
            Logger::log_info("D-Bus returned an unexpected value!\n\tIndex: %d, value: %d", index, dbus_message_iter_get_arg_type(&msg_iter[index]));
            dbus_error_free(&error);
            dbus_connection_unref(p_conn);
        return Err(ERR_NOT_SUPPORTED, "D-Bus returned an unexpected value.");
        }
    }

    uint32_t result = 0;
    dbus_message_iter_get_basic(&msg_iter[2], reinterpret_cast<void*>(&result));

    dbus_message_unref(p_resp);
    dbus_connection_unref(p_conn);

    return Ok(result == 1);
}

#else

bool RCalc::ImGuiStandardBackend::is_dark_theme() const { return Err(ERR_NOT_SUPPORTED, "RCalc must be compiled with D-Bus support on Linux for system theme."); }

#endif

#endif