#define window_frame_init
/// ():_
//gml_pragma("global", "global.
var b = buffer_create(32, buffer_grow, 1);
buffer_write(b, buffer_s32, window_get_x());
buffer_write(b, buffer_s32, window_get_y());
buffer_write(b, buffer_s32, window_get_width());
buffer_write(b, buffer_s32, window_get_height());
buffer_write(b, buffer_string, window_get_caption());
var r = window_frame_init_raw(window_handle(), buffer_get_address(b));
buffer_delete(b);
return r;