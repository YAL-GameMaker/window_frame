#include "window_frame.h"

bool window_frame_set_rect_impl(int x, int y, int w, int h, bool show) {
	HWND hwnd = active_hwnd;
	//
	RECT rect{};
	rect.left = x; rect.right = x + w;
	rect.top = y; rect.bottom = y + h;
	AdjustWindowRect(&rect, GetWindowLong(hwnd, GWL_STYLE), false);
	x = rect.left; w = rect.right - x;
	y = rect.top; h = rect.bottom - y;
	//
	if (show) {
		return SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_SHOWWINDOW);
	} else return MoveWindow(hwnd, x, y, w, h, TRUE);
}

dllg bool window_frame_set_region(int x, int y, int width, int height) {
	if (frame_bound) {
		MoveWindow(game_hwnd, x, y, width, height, TRUE);
		return true;
	} else return false;
}

///
dllx double window_frame_get_width() {
	RECT rect = window_rect();
	auto w = rect.right - rect.left;
	return w;
}
///
dllx double window_frame_get_height() {
	RECT rect = window_rect();
	auto h = rect.bottom - rect.top;
	return h;
}
///
dllx double window_frame_get_x() {
	RECT rect = window_rect();
	return rect.left;
}
///
dllx double window_frame_get_y() {
	RECT rect = window_rect();
	return rect.top;
}

dllg tuple<int, int, int, int> window_frame_get_rect() {
	auto rect = window_rect();
	return tuple<int, int, int, int>(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

dllg bool window_frame_set_rect(int x, int y, int width, int height) {
	return window_frame_set_rect_impl(x, y, width, height, false) != 0;
}

dllg bool window_frame_set_min_size(int minWidth, int minHeight) {
	WindowFrameCall(WindowFrameCall_t::SetMinWidth, minWidth);
	WindowFrameCall(WindowFrameCall_t::SetMinHeight, minHeight);
	return true;
}

dllg bool window_frame_set_max_size(int maxWidth, int maxHeight) {
	WindowFrameCall(WindowFrameCall_t::SetMaxWidth, maxWidth);
	WindowFrameCall(WindowFrameCall_t::SetMaxHeight, maxHeight);
	return true;
}