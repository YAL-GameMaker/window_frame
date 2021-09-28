#include "window_frame.h"
///
/*dllg tuple<double, double, double, double> window_frame_get_rect() {
	return tuple(

	);
}*/
/// (buffer_addr:ptr)->bool
dllx double window_frame_get_handle(HWND* out) {
	*(uint64_t*)out = 0;
	*out = frame_hwnd;
	return true;
}

// what a disaster
static void htoa10(HANDLE h, char* str) {
	auto n = (LONG_PTR)h;
	char* ptr = str;
	char* head = str;

	if (n == 0) {
		*ptr++ = '0';
		*ptr = 0;
		return;
	}

	bool neg = n < 0;
	if (neg) n = -n;

	while (n) {
		*ptr++ = '0' + (n % 10);
		n /= 10;
	}
	if (neg) *ptr++ = '-';
	*ptr = 0;

	for (--ptr; head < ptr; ++head, --ptr) {
		char tmp = *head;
		*head = *ptr;
		*ptr = tmp;
	}
}
///
dllx const char* window_frame_get_wid() {
	static char buf[32];
	htoa10(frame_hwnd, buf);
	return buf;
}

dllx double window_get_topmost_raw() {
	return (GetWindowLong(active_hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
}
dllx double window_set_topmost_raw(double stayontop) {
	//trace("topmost=%d", (int)stayontop);
	auto after = stayontop > 0.5 ? HWND_TOPMOST : HWND_NOTOPMOST;
	SetWindowPos(active_hwnd, after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return true;
}

///
dllg double window_frame_set_background(int color) {
	WindowFrameCall(WindowFrameCall_t::SetBackgroundColor, color);
	return true;
}

///
dllx double window_is_maximized() {
	return IsZoomed(active_hwnd);
}

dllx double window_frame_redraw() {
	RedrawWindow(frame_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	return true;
}

dllx double window_frame_force_focus() {
	// this may sound odd, but the only way to *truly* focus a window appears to be
	// to simulate a click on it.
	//SetFocus(frame_hwnd);
	POINT oldPos;
	auto hasOldPos = GetCursorPos(&oldPos);
	RECT clientRect;
	GetClientRect(game_hwnd, &clientRect);
	POINT clientTL;
	clientTL.x = clientRect.left;
	clientTL.y = clientRect.top;
	ClientToScreen(game_hwnd, &clientTL);
	SetCursorPos(clientTL.x + 1, clientTL.y + 1);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(oldPos.x, oldPos.y);
	return true;
}