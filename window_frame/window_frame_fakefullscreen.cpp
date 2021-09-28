#include "window_frame.h"
// added by Samuel V. / time-killer-games

bool window_frame_fakefullscreen = false;

RECT rectprev; RECT clientprev; LONG styleprev;
bool window_frame_set_fakefullscreen_impl(bool full) {
	if (full == window_frame_fakefullscreen) return true;
	auto hwnd = game_hwnd;
	auto fwnd = frame_hwnd;
	auto rect = window_rect();
	//trace("%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
	if (full) {
		rectprev = rect;
		GetClientRect(hwnd, &clientprev);
		styleprev = GetWindowLong(fwnd, GWL_STYLE);
		SetWindowLong(fwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CHILD);
		MoveWindow(fwnd, 0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), true);
		MoveWindow(hwnd, 0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), true);
	} else {
		SetWindowLong(fwnd, GWL_STYLE, styleprev & ~WS_CHILD);
		SetWindowPos(fwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE);
		AdjustWindowRect(&rectprev, GetWindowLong(fwnd, GWL_STYLE), false);
		MoveWindow(fwnd, rectprev.left, rectprev.top,
			rectprev.right - rectprev.left, rectprev.bottom - rectprev.top, true);
		SetWindowPos(hwnd, nullptr,
			(GetSystemMetrics(SM_CXSCREEN) / 2) - (clientprev.right / 2),
			(GetSystemMetrics(SM_CYSCREEN) / 2) - (clientprev.bottom / 2),
			clientprev.right, clientprev.bottom, 0);
	}
	SetFocus(hwnd);
	window_frame_fakefullscreen = full;
	return true;
}
///
dllx double window_frame_get_fakefullscreen() {
	return window_frame_fakefullscreen;
}
///
dllx double window_frame_set_fakefullscreen(double full) {
	return window_frame_set_fakefullscreen_impl(full > 0.5);
}