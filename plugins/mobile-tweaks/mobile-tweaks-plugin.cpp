#include "wayfire/plugin.hpp"
#include "wayfire/core.hpp"
#include "wayfire/output-layout.hpp"
#include "wayfire/signal-definitions.hpp"
#include <wayfire/util/log.hpp>

#include "dpms-ctrl.hpp"

class wayfire_mobile_tweaks_plugin : public wf::plugin_interface_t
{
    dpms_ctrl dpms;

  public:
    void init() override
    {
        wf::get_core().output_layout->connect(&dpms.on_output_layout_conf_changed);
    }

    void fini() override
    {
        wf::get_core().output_layout->disconnect(&dpms.on_output_layout_conf_changed);
    }
};

DECLARE_WAYFIRE_PLUGIN(wayfire_mobile_tweaks_plugin);