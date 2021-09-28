#include "gml_ext.h"
extern double window_command_check(int button);
dllx double window_command_check_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_button;
	_arg_button = _in.read<int>();
	return window_command_check(_arg_button);
}

extern int window_command_run(int command, int lParam);
dllx double window_command_run_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_command;
	_arg_command = _in.read<int>();
	int _arg_lParam;
	if (_in.read<bool>()) {
		_arg_lParam = _in.read<int>();
	} else _arg_lParam = 0;
	return window_command_run(_arg_command, _arg_lParam);
}

extern bool window_command_hook(int button);
dllx double window_command_hook_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_button;
	_arg_button = _in.read<int>();
	return window_command_hook(_arg_button);
}

extern bool window_command_unhook(int button);
dllx double window_command_unhook_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_button;
	_arg_button = _in.read<int>();
	return window_command_unhook(_arg_button);
}

extern bool window_command_get_hooked(int button);
dllx double window_command_get_hooked_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_button;
	_arg_button = _in.read<int>();
	return window_command_get_hooked(_arg_button);
}

extern bool window_command_set_hooked(int button, bool hook);
dllx double window_command_set_hooked_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_button;
	_arg_button = _in.read<int>();
	bool _arg_hook;
	_arg_hook = _in.read<bool>();
	return window_command_set_hooked(_arg_button, _arg_hook);
}

extern bool window_command_get_active(int command);
dllx double window_command_get_active_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_command;
	_arg_command = _in.read<int>();
	return window_command_get_active(_arg_command);
}

extern bool window_command_set_active(int command, bool val);
dllx double window_command_set_active_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_command;
	_arg_command = _in.read<int>();
	bool _arg_val;
	_arg_val = _in.read<bool>();
	return window_command_set_active(_arg_command, _arg_val);
}

extern bool window_frame_init(GAME_HWND hwnd, int x, int y, int w, int h, const char* title);
dllx double window_frame_init_raw(void* _ptr, const char* _arg_title) {
	gml_istream _in(_ptr);
	GAME_HWND _arg_hwnd;
	_arg_hwnd = (GAME_HWND)_in.read<uint64_t>();
	int _arg_x;
	_arg_x = _in.read<int>();
	int _arg_y;
	_arg_y = _in.read<int>();
	int _arg_w;
	_arg_w = _in.read<int>();
	int _arg_h;
	_arg_h = _in.read<int>();
	return window_frame_init(_arg_hwnd, _arg_x, _arg_y, _arg_w, _arg_h, _arg_title);
}

extern double window_frame_set_background(int color);
dllx double window_frame_set_background_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_color;
	_arg_color = _in.read<int>();
	return window_frame_set_background(_arg_color);
}

extern bool window_frame_set_region(int x, int y, int width, int height);
dllx double window_frame_set_region_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_x;
	_arg_x = _in.read<int>();
	int _arg_y;
	_arg_y = _in.read<int>();
	int _arg_width;
	_arg_width = _in.read<int>();
	int _arg_height;
	_arg_height = _in.read<int>();
	return window_frame_set_region(_arg_x, _arg_y, _arg_width, _arg_height);
}

extern tuple<int, int, int, int> window_frame_get_rect();
dllx double window_frame_get_rect_raw(void* _ptr) {
	gml_istream _in(_ptr);
	tuple<int, int, int, int> _ret = window_frame_get_rect();
	gml_ostream _out(_ptr);
	_out.write_tuple<int, int, int, int>(_ret);
	return 1;
}

extern bool window_frame_set_rect(int x, int y, int width, int height);
dllx double window_frame_set_rect_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_x;
	_arg_x = _in.read<int>();
	int _arg_y;
	_arg_y = _in.read<int>();
	int _arg_width;
	_arg_width = _in.read<int>();
	int _arg_height;
	_arg_height = _in.read<int>();
	return window_frame_set_rect(_arg_x, _arg_y, _arg_width, _arg_height);
}

extern bool window_frame_set_min_size(int minWidth, int minHeight);
dllx double window_frame_set_min_size_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_minWidth;
	_arg_minWidth = _in.read<int>();
	int _arg_minHeight;
	_arg_minHeight = _in.read<int>();
	return window_frame_set_min_size(_arg_minWidth, _arg_minHeight);
}

extern bool window_frame_set_max_size(int maxWidth, int maxHeight);
dllx double window_frame_set_max_size_raw(void* _ptr) {
	gml_istream _in(_ptr);
	int _arg_maxWidth;
	_arg_maxWidth = _in.read<int>();
	int _arg_maxHeight;
	_arg_maxHeight = _in.read<int>();
	return window_frame_set_max_size(_arg_maxWidth, _arg_maxHeight);
}

