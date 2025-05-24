#pragma once

class dpms_ctrl
{
  public:
    dpms_ctrl();
    ~dpms_ctrl();
    
    wf::option_wrapper_t<int> ini_dpms_timeout{"mobile-tweaks/dpms_timeout"};
    wf::option_wrapper_t<bool> ini_wake_up_any{"mobile-tweaks/wake_up_any"};

    wf::signal::connection_t<wf::seat_activity_signal> on_seat_activity;
    wf::signal::connection_t<wf::input_event_signal<wlr_keyboard_key_event>> on_key_event;
    wf::signal::connection_t<wf::output_layout_configuration_changed_signal> on_output_layout_conf_changed;

    void set_state(wf::output_image_source_t to);

  private:
    wf::wl_timer<false> m_idleTimeOut;
    wf::wl_timer<false> m_ignoreActivity;
    
    uint32_t m_pwr_pressed_time;

    void create_dpms_timeout();
    void handle_seat_activity();
    void handle_key_event(wf::input_event_signal<wlr_keyboard_key_event> *ev);
    void handle_output_config_changed();
    void updateIdleHint();
    bool m_idle;
};