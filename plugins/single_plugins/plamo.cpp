#include "wayfire/bindings.hpp"
#include "wayfire/plugin.hpp"
#include "wayfire/plugins/common/shared-core-data.hpp"
#include "wayfire/signal-provider.hpp"
#include <cstdint>
#include <wayfire/config/option-types.hpp>
#include <wayfire/config/types.hpp>
#include <wayfire/per-output-plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/core.hpp>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/bindings-repository.hpp>
#include <wayfire/seat.hpp>
#include <wayfire/util/log.hpp>
#include <plugins/ipc/ipc-method-repository.hpp>
#include <set>

class wayfire_plamo_plugin : public wf::plugin_interface_t
{
  public:
    void init()
    {
        method_repository->register_method("plamo/watch", on_client_watch);
        method_repository->connect(&on_client_disconnected);
        wf::get_core().connect(&on_key_event);
    }

    void fini()
    {
        method_repository->unregister_method("plamo/watch");
        wf::get_core().disconnect(&on_key_event);
    }

    wf::ipc::method_callback_full on_client_watch =
        [=] (nlohmann::json data, wf::ipc::client_interface_t *client)
    {
        clients.insert(client);
        return wf::ipc::json_ok();
    };

  private:
    wf::shared_data::ref_ptr_t<wf::ipc::method_repository_t> method_repository;
    std::set<wf::ipc::client_interface_t*> clients;

    wf::signal::connection_t<wf::ipc::client_disconnected_signal> on_client_disconnected =
        [=] (wf::ipc::client_disconnected_signal *ev)
    {
        clients.erase(ev->client);
    };

    wf::signal::connection_t<wf::input_event_signal<wlr_keyboard_key_event>> on_key_event =
        [=] (wf::input_event_signal<wlr_keyboard_key_event> *ev)
    {
        if ((ev->event->keycode != KEY_POWER))
            return;
        
        if (ev->event->state == WLR_KEY_PRESSED)
        {
            nlohmann::json event;
            event["event"] = "power-key-pressed";
            for (auto& client : clients)
            {
                client->send_json(event);
            }
        } else
        {
            nlohmann::json event;
            event["event"] = "power-key-released";
            for (auto& client : clients)
            {
                client->send_json(event);
            }
        }
    };
};

DECLARE_WAYFIRE_PLUGIN(wayfire_plamo_plugin);