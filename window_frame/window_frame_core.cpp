/// window_frame.cpp
/// @author YellowAfterlife

#include "stdafx.h"
#include "window_frame.h"
HWND game_hwnd, frame_hwnd;
bool frame_bound;

RECT window_rect() {
	auto hwnd = active_hwnd;
	RECT windowRect, clientRect, adjustedRect{};
	GetClientRect(hwnd, &clientRect);
	adjustedRect.left = clientRect.left; adjustedRect.right = clientRect.right;
	adjustedRect.top = clientRect.top; adjustedRect.bottom = clientRect.bottom;
	AdjustWindowRect(&adjustedRect, GetWindowLong(hwnd, GWL_STYLE), false);
	GetWindowRect(hwnd, &windowRect);
	windowRect.left -= (adjustedRect.left - clientRect.left);
	windowRect.top -= (adjustedRect.top - clientRect.top);
	windowRect.right -= (adjustedRect.right - clientRect.right);
	windowRect.bottom -= (adjustedRect.bottom - clientRect.bottom);
	return windowRect;
}

bool window_frame_set_visible_impl(bool show, bool setvis) {
	if (show == frame_bound) return true;
	auto hwnd = game_hwnd;
	auto fwnd = frame_hwnd;
	auto rect = window_rect();
	//trace("%d,%d,%d,%d", rect.left, rect.top, rect.right, rect.bottom);
	if (show) {
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_POPUP | WS_CHILD);
		SetParent(hwnd, fwnd);
		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE);
		AdjustWindowRect(&rect, GetWindowLong(fwnd, GWL_STYLE), false);
		SetWindowPos(fwnd, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
		if (setvis) {
			ShowWindow(fwnd, SW_SHOW);
			RedrawWindow(frame_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
		}
	} else {
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CHILD | WS_POPUP);
		SetParent(hwnd, nullptr);
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
		if (setvis) ShowWindow(fwnd, SW_HIDE);
	}
	SetFocus(hwnd);
	frame_bound = show;
	return true;
}
dllx double window_frame_set_visible_raw(double visible) {
	return window_frame_set_visible_impl(visible > 0.5, true);
}


/// ->bool
dllx double window_frame_set_caption(const char* text) {
	wchar_t wtext[1025];
	MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, std::size(wtext) - 1);
	wtext[std::size(wtext) - 1] = 0;
	return SetWindowTextW(frame_hwnd, wtext);
}

/// ->bool
dllx double window_frame_sync_icons() {
	auto _game_hwnd = game_hwnd;
	auto _frame_hwnd = frame_hwnd;
	if (!_frame_hwnd) return false;
	SendMessage(_frame_hwnd, WM_SETICON, ICON_SMALL2, SendMessage(_game_hwnd, WM_GETICON, ICON_SMALL2, 0));
	SendMessage(_frame_hwnd, WM_SETICON, ICON_SMALL,  SendMessage(_game_hwnd, WM_GETICON, ICON_SMALL,  0));
	SendMessage(_frame_hwnd, WM_SETICON, ICON_BIG,    SendMessage(_game_hwnd, WM_GETICON, ICON_BIG,    0));
	return true;
}

dllx double window_frame_preinit_raw() {
	return true;
}
