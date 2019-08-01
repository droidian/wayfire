#ifndef WF_SEAT_POINTER_HPP
#define WF_SEAT_POINTER_HPP

#include <nonstd/observer_ptr.h>
#include <surface.hpp>
#include <util.hpp>
#include <config.hpp>
#include "surface-map-state.hpp"

extern "C"
{
struct wlr_pointer_constraint_v1;
#include <wlr/types/wlr_seat.h>
}

class input_manager;
namespace wf
{
/**
 * Represents the "mouse cursor" part of a wf_cursor, i.e functionality provided
 * by touchpads, regular mice, trackpoints and similar.
 *
 * It is responsible for managing the focused surface and processing input
 * events from the aforementioned devices.
 */
class LogicalPointer
{
  public:
    LogicalPointer(nonstd::observer_ptr<input_manager> input);
    ~LogicalPointer();

    /**
     * Handle an update of the cursor's position, which includes updating the
     * surface currently under the pointer.
     *
     * @param time_msec The time when the event causing this update occured
     * @param real_update Whether the update is caused by a hardware event or
     *                    was artifically generated.
     */
    void update_cursor_position(uint32_t time_msec, bool real_update = true);

    /** Get the currenntlly set cursor focus */
    wf::surface_interface_t *get_focus() const;

    /**
     * Set the active pointer constraint
     *
     * @param last_destroyed In case a constraint is destroyed, the constraint
     * should be set to NULL, but this requires special handling, because not
     * all operations are supported on destroyed constraints
     */
    void set_pointer_constraint(wlr_pointer_constraint_v1 *constraint,
        bool last_destroyed = false);

    /**
     * Calculate the point inside the constraint region closest to the given
     * point.
     *
     * @param point The point to be constrained inside the region.
     * @return The closest point
     */
    wf_pointf constrain_point(wf_pointf point);

    /** @return The currently active pointer constraint */
    wlr_pointer_constraint_v1 *get_active_pointer_constraint();

    /** Handle events coming from the input devices */
    void handle_pointer_axis  (wlr_event_pointer_axis *ev);
    void handle_pointer_motion(wlr_event_pointer_motion *ev);
    void handle_pointer_motion_absolute(wlr_event_pointer_motion_absolute *ev);
    void handle_pointer_button(wlr_event_pointer_button *ev);

    /** Handle touchpad gestures detected by libinput */
    void handle_pointer_swipe_begin(wlr_event_pointer_swipe_begin *ev);
    void handle_pointer_swipe_update(wlr_event_pointer_swipe_update *ev);
    void handle_pointer_swipe_end(wlr_event_pointer_swipe_end *ev);
    void handle_pointer_pinch_begin(wlr_event_pointer_pinch_begin *ev);
    void handle_pointer_pinch_update(wlr_event_pointer_pinch_update *ev);
    void handle_pointer_pinch_end(wlr_event_pointer_pinch_end *ev);
    void handle_pointer_frame();

    /** Whether there are pressed buttons currently */
    bool has_pressed_buttons() const;

  private:
    nonstd::observer_ptr<input_manager> input;
    SurfaceMapStateListener on_surface_map_state_change;

    /** The surface which currently has cursor focus */
    wf::surface_interface_t* cursor_focus = nullptr;

    /**
     * Set the pointer focus.
     *
     * @param surface The surface which should receive focus
     * @param local   The coordinates of the pointer relative to surface.
     *                No meaning if the surface is nullptr
     */
    void update_cursor_focus(wf::surface_interface_t *surface, wf_pointf local);

    /** Number of currently-pressed mouse buttons */
    int count_pressed_buttons = 0;
    wf_region constraint_region;
    wlr_pointer_constraint_v1 *active_pointer_constraint = nullptr;

    /** Figure out the global position of the given point.
     * @param relative The point relative to the cursor focus */
    wf_pointf get_absolute_position_from_relative(wf_pointf relative);

    wf_option mouse_scroll_speed;
    wf_option touchpad_scroll_speed;

    /** Check whether an implicit grab should start/end */
    void check_implicit_grab();

    /** Implicitly grabbed surface when a button is being held */
    wf::surface_interface_t *grabbed_surface = nullptr;

    /** Set the currently grabbed surface
     * @param surface The surface to be grabbed, or nullptr to reset grab */
    void grab_surface(wf::surface_interface_t *surface);

    /** Send a button event to the currently active receiver, i.e to the
     * active input grab(if any), or to the focused surface */
    void send_button(wlr_event_pointer_button *ev, bool has_binding);
};
}

#endif /* end of include guard: WF_SEAT_POINTER_HPP */
