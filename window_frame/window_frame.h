#pragma once
#include "stdafx.h"
extern HWND game_hwnd, frame_hwnd;
extern bool frame_bound;
#define active_hwnd (frame_bound ? frame_hwnd : game_hwnd)

inline HWND GetWindowFramePair(HWND window) {
	return (HWND)GetWindowLongPtrW(window, GWLP_USERDATA);
}
inline void SetWindowFramePair(HWND window, HWND pair) {
	SetWindowLongPtrW(window, GWLP_USERDATA, (LONG_PTR)pair);
}

extern yal_set<WPARAM> queued_syscommands;
extern yal_set<WPARAM> game_blocked_syscommands;
extern yal_set<WPARAM> game_hooked_syscommands;
extern bool game_hook_syscommands;
RECT window_rect();

bool window_frame_set_visible_impl(bool show, bool setvis);
bool window_frame_set_rect_impl(int x, int y, int w, int h, bool show);

#define dllnx dllx
/// ->bool
dllnx double window_frame_sync_icons();
/// ->bool
dllnx double window_frame_set_caption(const char* text);
#undef dllnx

#define WM_WF_SYSCOMMAND (WM_APP + 1)
// game: mark hooked command, frame: run command
#define WM_WF_CALL (WM_APP + 2)
enum class WindowFrameCall_t {
	DisableSysCommand = 1,
	EnableSysCommand,
	IsSysCommandEnabled,

	HookSysCommand,
	UnhookSysCommand,
	IsSysCommandHooked,

	SetBackgroundColor,
	SetMinWidth,
	SetMinHeight,
	SetMaxWidth,
	SetMaxHeight,

	SetStdOut,
};

inline LRESULT WindowFrameCall(WindowFrameCall_t conf, LPARAM lParam) {
	return SendMessageW(frame_hwnd, WM_WF_CALL, (WPARAM)conf, (LPARAM)lParam);
}