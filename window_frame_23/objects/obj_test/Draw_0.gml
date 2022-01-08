tm_start();

/* these are equivalent to
if (tm_button("...")) {
	window_command_set_active(window_command_close, !window_command_get_active(window_command_close));
}
*/
tm_command_toggle(window_command_close, "Close enabled");
tm_command_toggle(window_command_minimize, "Minimize enabled");
tm_command_toggle(window_command_maximize, "Maximize enabled");
tm_command_toggle(window_command_resize, "Resize enabled");
if (tm_toggle("Restore/un-max", window_is_maximized())) {
	window_command_run(window_command_restore);
}

/*
these are equivalent to
if (tm_button("...")) {
	window_command_set_hooked(window_command_close, !window_command_get_hooked(window_command_close));
}
and doing
if (window_command_check(window_command_close)) { ... }
once per frame
*/
tm_start(5 + (tm_width + 15));
tm_command_hook(window_command_close, "Hook close");
tm_command_hook(window_command_minimize, "Hook minimize");
tm_command_hook(window_command_maximize, "Hook maximize");
tm_command_hook(window_command_restore, "Hook restore");

tm_start(5 + (tm_width + 15) * 2);
if (tm_toggle("Frame enabled", window_frame_get_visible())) {
	window_frame_set_visible(!window_frame_get_visible());
}
if (tm_toggle("Aspect scale", keep_aspect)) keep_aspect = !keep_aspect;
if (tm_toggle("Stay on top", window_get_topmost())) {
	window_set_topmost(!window_get_topmost());
}
if (tm_toggle("Fullscreen", window_frame_get_fullscreen())) {
	window_frame_set_fullscreen(!window_frame_get_fullscreen());
}
//if (tm_button("Random bg color")) window_frame_set_background(make_color_hsv(random(256), 200, 250));
tm_button("Time: " + string_format(current_time/1000, 0, 1));