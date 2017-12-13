#define window_frame_init
/// ()
gml_pragma("global", "window_frame_init()");
global.window_frame_await = 0;
global.window_frame_ready = false;
global.window_frame_bound = false;
global.window_frame_visible = false;
global.window_frame_buffer = -1;

#define window_frame_update
/// (): Should be called once per frame.
if (global.window_frame_await < 2 && ++global.window_frame_await >= 2) {
	var b = global.window_frame_buffer;
	if (b == -1) {
		b = buffer_create(32, buffer_grow, 1);
		global.window_frame_buffer = b;
	}
	buffer_seek(b, buffer_seek_start, 0);
	buffer_write(b, buffer_s32, window_get_x());
	buffer_write(b, buffer_s32, window_get_y());
	buffer_write(b, buffer_s32, window_get_width());
	buffer_write(b, buffer_s32, window_get_height());
	buffer_write(b, buffer_string, window_get_caption());
	if (window_frame_init_raw(window_handle(), buffer_get_address(b))) {
		global.window_frame_ready = true;
		global.window_frame_bound = true;
		global.window_frame_visible = true;
	}
	buffer_delete(b);
}
if (global.window_frame_ready) {
	// unbind when in full-screen mode, bind back when windowed.
	if (window_get_fullscreen()) {
		if (global.window_frame_bound) {
			global.window_frame_bound = false;
			window_frame_set_visible_raw(false);
		}
	} else {
		if (global.window_frame_visible && !global.window_frame_bound) {
			global.window_frame_bound = true;
			window_frame_set_visible(true);
		}
	}
}

#define window_frame_get_visible
/// (): Returns whether the frame is currently on.
return global.window_frame_visible;

#define window_frame_set_visible
/// (): Turns the frame on/off.
var z = argument0;
global.window_frame_visible = z;
if (!window_get_fullscreen()) {
	window_frame_set_visible_raw(z);
	global.window_frame_bound = z;
}

#define window_frame_get_rect
/// (): Returns an array of x, y, width, height
var b = global.window_frame_buffer;
var r = array_create(4);
if (b != -1) {
	window_frame_get_rect_raw(buffer_get_address(b));
	buffer_seek(b, buffer_seek_start, 0);
	r[0] = buffer_read(b, buffer_s32);
	r[1] = buffer_read(b, buffer_s32);
	r[2] = buffer_read(b, buffer_s32);
	r[3] = buffer_read(b, buffer_s32);
}
return r;

#define window_frame_set_fullscreen
/// (full): 
if (argument0) {
	if (global.window_frame_bound) {
		global.window_frame_bound = false;
		window_frame_set_visible_raw(false);
	}
	window_set_fullscreen(true);
} else {
	window_set_fullscreen(false);
	if (global.window_frame_visible) {
		global.window_frame_bound = true;
		window_frame_set_visible(true);
	}
}

#define window_frame_cleanup
window_frame_cleanup_raw();
if (global.window_frame_buffer != -1) {
	buffer_delete(global.window_frame_buffer);
	global.window_frame_buffer = -1;
}