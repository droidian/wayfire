#include "wayfire/core.hpp"
#include "wayfire/output-layout.hpp"
#include "wayfire/signal-definitions.hpp"
#include <wayfire/util/log.hpp>

#include <linux/input-event-codes.h>
#include <gio/gio.h>
#include "dpms-ctrl.hpp"

dpms_ctrl::dpms_ctrl()
{
    on_output_layout_conf_changed =
        [=] (wf::output_layout_configuration_changed_signal *ev)
    {
        handle_output_config_changed();
    };

    on_seat_activity = [=] (void*)
    {
        handle_seat_activity();
    };

    on_key_event =
        [=] (wf::input_event_signal<wlr_keyboard_key_event> *ev)
    {
        handle_key_event(ev);
    };

    wf::get_core().connect(&on_seat_activity);
    wf::get_core().connect(&on_key_event);
}

dpms_ctrl::~dpms_ctrl()
{
    wf::get_core().disconnect(&on_seat_activity);
    wf::get_core().disconnect(&on_key_event);
}

void dpms_ctrl::create_dpms_timeout()
{
    m_idleTimeOut.set_timeout(60000, [=] ()
    {
        set_state(wf::OUTPUT_IMAGE_SOURCE_DPMS);
    });
}

void dpms_ctrl::set_state(wf::output_image_source_t to)
{
    auto config = wf::get_core().output_layout->get_current_configuration();

    for (auto& entry : config)
    {
        if (entry.second.scale > 1.0)
            entry.second.source = to;
    }
    wf::get_core().output_layout->apply_configuration(config);
}

void dpms_ctrl::handle_seat_activity()
{
    if(m_ignoreActivity.is_connected())
        return;

    if(m_idle)
        set_state(wf::OUTPUT_IMAGE_SOURCE_SELF);

    m_idleTimeOut.disconnect();
    create_dpms_timeout();
}

void dpms_ctrl::handle_key_event(wf::input_event_signal<wlr_keyboard_key_event> *ev)
{
    if ((ev->event->keycode != KEY_POWER))
        return;

    if (ev->event->state == WLR_KEY_PRESSED)
    {
        m_ignoreActivity.set_timeout(1000, [=] (){});
        m_pwr_pressed_time = wf::get_current_time();
    } else if ((wf::get_current_time() - m_pwr_pressed_time < 250))
    {
        m_ignoreActivity.set_timeout(1000, [=] (){});
        if(!m_idle)
            set_state(wf::OUTPUT_IMAGE_SOURCE_DPMS);
        else
            set_state(wf::OUTPUT_IMAGE_SOURCE_SELF);
    }
}

void dpms_ctrl::handle_output_config_changed()
{
    auto config = wf::get_core().output_layout->get_current_configuration();
    for (auto& entry : config)
    {
        if (entry.second.scale > 1.0)
        {
            if (entry.second.source == wf::OUTPUT_IMAGE_SOURCE_DPMS)
            {
                m_idleTimeOut.disconnect();
                m_idle = true;
            } else
            {
                create_dpms_timeout();
                m_idle = false;
            }
        }
    }
    updateIdleHint();
}

void dpms_ctrl::updateIdleHint()
{
    GDBusConnection *conn;
    GError *error = NULL;

    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);

    if (error)
    {
        LOGE("Failed to connect to D-Bus session.");
        return;
    }

    g_dbus_connection_call_sync (conn,
       "org.freedesktop.login1",
       "/org/freedesktop/login1/session/self",
       "org.freedesktop.login1.Session",
       "SetIdleHint",
       g_variant_new ("(b)",
        m_idle),
       NULL,
       G_DBUS_CALL_FLAGS_NONE,
       -1,
       NULL,
       &error);

    if (error)
    {
        LOGE("Failed to set IdleHint");
    }
}