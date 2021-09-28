window_frame_update();
if (window_frame_get_visible()) {
	if (keep_aspect) {
		event_user(1);
	} else event_user(0);
}

if (surface_exists(application_surface) && (
	surface_get_width(application_surface) != room_width
	|| surface_get_height(application_surface) != room_height
)) surface_resize(application_surface, room_width, room_height);