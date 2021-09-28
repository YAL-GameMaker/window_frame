#define gamepad_force_focus
/// (enable=true)
var enable = argument_count > 0 ? argument[0] : true;
return gamepad_force_focus_raw(window_handle(), enable);