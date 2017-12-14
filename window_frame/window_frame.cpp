/// @author YellowAfterlife

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <map>
#include "window_frame_interop.h"

#define dllx extern "C" __declspec(dllexport)
#define trace(...) { printf(__VA_ARGS__); printf("\n"); fflush(stdout); }

struct buffer {
	char* pos;
	public:
	buffer(char* origin) : pos(origin) {}
	template<class T> T read() {
		T r = *(T*)pos;
		pos += sizeof(T);
		return r;
	}
	template<class T> void write(T val) {
		*(T*)pos = val;
		pos += sizeof(T);
	}
	//
	char* read_string() {
		char* r = pos;
		while (*pos != 0) pos++;
		pos++;
		return r;
	}
	void write_string(const char* s) {
		for (int i = 0; s[i] != 0; i++) write<char>(s[i]);
		write<char>(0);
	}
};

class StringConv {
	public:
	char* cbuf = NULL;
	size_t cbuf_size = 0;
	WCHAR* wbuf = NULL;
	size_t wbuf_size = 0;
	StringConv() {

	}
	LPCWSTR wget(size_t size) {
		if (wbuf_size < size) {
			if (wbuf != NULL) delete wbuf;
			wbuf = new WCHAR[size];
			wbuf_size = size;
		}
		return wbuf;
	}
	LPCWSTR proc(char* src, int cp = CP_UTF8) {
		size_t size = MultiByteToWideChar(cp, 0, src, -1, NULL, 0);
		LPCWSTR buf = wget(size);
		MultiByteToWideChar(cp, 0, src, -1, wbuf, size);
		return wbuf;
	}
	char* get(size_t size) {
		if (cbuf_size < size) {
			if (cbuf != NULL) delete cbuf;
			cbuf = new char[size];
			cbuf_size = size;
		}
		return cbuf;
	}
	char* proc(LPCWSTR src, int cp = CP_UTF8) {
		size_t size = WideCharToMultiByte(cp, 0, src, -1, NULL, 0, NULL, NULL);
		char* buf = get(size);
		WideCharToMultiByte(cp, 0, src, -1, buf, size, NULL, NULL);
		return buf;
	}
} utf8;

HWND window_frame_host_hwnd = nullptr;
HANDLE window_frame_host_proc = nullptr;
HWND window_frame_game_hwnd = nullptr;
bool window_frame_bound = false;
//
std::map<WPARAM, bool> window_command_hooks;
std::map<WPARAM, bool> window_command_blocks;
#define window_frame_host_config(type, lParam) SendMessage(window_frame_host_hwnd, WFI_WM_HOST_CONFIG, type, lParam);

BOOL CALLBACK window_frame_enum_wnds(HWND hwnd, LPARAM param) {
	DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
	if (thread == (DWORD)param) {
		window_frame_host_hwnd = hwnd;
		return FALSE;
	} else return TRUE;
}

WNDPROC window_frame_wndproc_base = nullptr;
LRESULT window_frame_wndproc_hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_ERASEBKGND:
			return TRUE;
		case WFI_WM_CLIENT_NOTIFY_HOOK: // notify hook
			window_command_hooks[wp] = true;
			return TRUE;
	}
	return CallWindowProc(window_frame_wndproc_base, hwnd, msg, wp, lp);
}

RECT window_frame_get_rect() {
	HWND hwnd = window_frame_host_hwnd;
	RECT wr, cr, ar;
	GetClientRect(hwnd, &cr);
	ar.left = cr.left; ar.right = cr.right;
	ar.top = cr.top; ar.bottom = cr.bottom;
	AdjustWindowRect(&ar, GetWindowLong(hwnd, GWL_STYLE), false);
	GetWindowRect(hwnd, &wr);
	wr.left -= (ar.left - cr.left);
	wr.top -= (ar.top - cr.top);
	wr.right -= (ar.right - cr.right);
	wr.bottom -= (ar.bottom - cr.bottom);
	return wr;
}
bool window_frame_set_visible_impl(bool show, bool setvis) {
	if (show == window_frame_bound) return true;
	HWND hwnd = window_frame_game_hwnd;
	HWND fwnd = window_frame_host_hwnd;
	if (show) {
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_POPUP | WS_CHILD);
		SetParent(hwnd, fwnd);
		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE);
		if (setvis) ShowWindow(fwnd, SW_SHOW);
	} else {
		RECT rect = window_frame_get_rect();
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CHILD | WS_POPUP);
		SetParent(hwnd, nullptr);
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
		if (setvis) ShowWindow(fwnd, SW_HIDE);
	}
	SetFocus(hwnd);
	window_frame_bound = show;
	return true;
}
dllx double window_frame_set_visible_raw(double visible) {
	return window_frame_set_visible_impl(visible > 0.5, true);
}

#pragma region Window Rect
BOOL window_frame_set_rect_impl(int x, int y, int w, int h, bool show) {
	//
	RECT rect;
	rect.left = x; rect.right = x + w;
	rect.top = y; rect.bottom = y + h;
	AdjustWindowRect(&rect, GetWindowLong(window_frame_host_hwnd, GWL_STYLE), false);
	x = rect.left; w = rect.right - x;
	y = rect.top; h = rect.bottom - y;
	//
	if (show) {
		return SetWindowPos(window_frame_host_hwnd, nullptr, x, y, w, h, SWP_SHOWWINDOW);
	} else return MoveWindow(window_frame_host_hwnd, x, y, w, h, TRUE);
}

///
dllx double window_frame_get_width() {
	RECT rect = window_frame_get_rect();
	return rect.right - rect.left;
}
///
dllx double window_frame_get_height() {
	RECT rect = window_frame_get_rect();
	return rect.bottom - rect.top;
}
///
dllx double window_frame_get_x() {
	RECT rect = window_frame_get_rect();
	return rect.left;
}
///
dllx double window_frame_get_y() {
	RECT rect = window_frame_get_rect();
	return rect.top;
}
dllx double window_frame_get_rect_raw(char* cbuf) {
	buffer buf(cbuf);
	RECT rect = window_frame_get_rect();
	buf.write<INT32>(rect.left);
	buf.write<INT32>(rect.top);
	buf.write<INT32>(rect.right - rect.left);
	buf.write<INT32>(rect.bottom - rect.top);
	return true;
}
///
dllx double window_frame_set_rect(double x, double y, double w, double h) {
	return window_frame_set_rect_impl((int)x, (int)y, (int)w, (int)h, false) != 0;
}
#pragma endregion

#pragma region Window Caption
///
dllx double window_frame_set_region(double x, double y, double w, double h) {
	if (window_frame_bound) {
		MoveWindow(window_frame_game_hwnd, (int)x, (int)y, (int)w, (int)h, TRUE);
		return true;
	} else return false;
}
///
dllx double window_frame_set_caption(char* text) {
	return SetWindowText(window_frame_host_hwnd, utf8.proc(text));
}
#pragma endregion

#pragma region Window Commands
dllx double window_command_run_raw(double wp, double lp) {
	return SendMessage(window_frame_host_hwnd, WFI_WM_HOST_EXEC_SYSCOMMAND, (WPARAM)wp, (LPARAM)lp);
}
dllx double window_command_hook_raw(double button) {
	auto wb = (WPARAM)button;
	if (window_command_hooks.find(wb) != window_command_hooks.end()) return 1;
	window_command_hooks[wb] = false;
	window_frame_host_config(WFI_HOST_CONF_HOOK_SYSCOMMAND, wb);
	return 1;
}
dllx double window_command_unhook_raw(double button) {
	auto wb = (WPARAM)button;
	auto q = window_command_hooks.find(wb);
	if (q != window_command_hooks.end()) {
		window_command_hooks.erase(q);
		window_frame_host_config(WFI_HOST_CONF_UNHOOK_SYSCOMMAND, wb);
	}
	return 1;
}
long window_command_long(double cmd) {
	switch ((WPARAM)cmd) {
		case SC_SIZE: return WS_THICKFRAME;
		case SC_MINIMIZE: return WS_MINIMIZEBOX;
		case SC_MAXIMIZE: return WS_MAXIMIZEBOX;
		default: return -1;
	}
}
int window_command_acc_active(double cmd, double _val) {
	auto hwnd = window_frame_host_hwnd;
	if (hwnd == nullptr) return -1;
	auto wcmd = (WPARAM)cmd;
	auto get = _val < 0;
	auto set = _val > 0;
	switch (wcmd) {
		case SC_MOVE: case SC_MOUSEMENU: {
			auto q = window_command_blocks.find(wcmd);
			if (get) return q == window_command_blocks.end();
			auto z = q != window_command_blocks.end();
			if (set) {
				if (z) {
					window_command_blocks.erase(q);
					window_frame_host_config(WFI_HOST_CONF_ALLOW_SYSCOMMAND, wcmd);
				}
			} else {
				if (!z) {
					window_command_blocks[wcmd] = true;
					window_frame_host_config(WFI_HOST_CONF_BLOCK_SYSCOMMAND, wcmd);
				}
			}
			return 1;
		}; break;
		case SC_CLOSE: {
			auto menu = GetSystemMenu(hwnd, false);
			if (get) return (GetMenuState(menu, wcmd, MF_BYCOMMAND) & MF_GRAYED) == 0;
			if (EnableMenuItem(menu, wcmd, MF_BYCOMMAND | (set ? MF_ENABLED : MF_GRAYED)) < 0) return -1;
			return 1;
		}; break;
		default: {
			auto cl = window_command_long(cmd);
			if (cl < 0) return -1;
			auto wl = GetWindowLong(hwnd, GWL_STYLE);
			if (get) return (wl & cl) == cl;
			if (set) wl |= cl; else wl &= ~cl;
			SetWindowLong(hwnd, GWL_STYLE, wl);
			return 1;
		}; break;
	}
}
dllx double window_command_get_active_raw(double cmd) {
	return window_command_acc_active(cmd, -1);
}
dllx double window_command_set_active_raw(double cmd, double val) {
	return window_command_acc_active(cmd, val > 0.5 ? 1 : 0);
}
/// Returns whether the given button was pressed since the last call to this function.
dllx double window_command_check(double button) {
	auto wb = (WPARAM)button;
	auto q = window_command_hooks.find(wb);
	if (q != window_command_hooks.end()) {
		if (q->second) {
			window_command_hooks[wb] = false;
			return 1;
		} else return 0;
	} else return 0;
}
#pragma endregion

#pragma region Extras
///
dllx double window_frame_set_background(double color) {
	window_frame_host_config(WFI_HOST_CONF_SET_BACKGROUND, (int)color);
	return true;
}
///
dllx double window_frame_set_min_size(double minWidth, double minHeight) {
	window_frame_host_config(WFI_HOST_CONF_SET_MIN_WIDTH, (int)minWidth);
	window_frame_host_config(WFI_HOST_CONF_SET_MIN_HEIGHT, (int)minHeight);
	return true;
}
///
dllx double window_frame_set_max_size(double maxWidth, double maxHeight) {
	window_frame_host_config(WFI_HOST_CONF_SET_MAX_WIDTH, (int)maxWidth);
	window_frame_host_config(WFI_HOST_CONF_SET_MAX_HEIGHT, (int)maxHeight);
	return true;
}
#pragma endregion


dllx double window_frame_init_raw(char* cwnd, char* cbuf) {
	HWND hwnd = (HWND)cwnd;
	window_frame_game_hwnd = hwnd;
	// read arguments:
	buffer data(cbuf);
	int x = data.read<INT32>();
	int y = data.read<INT32>();
	int w = data.read<INT32>();
	int h = data.read<INT32>();
	char* title = data.read_string();
	//
	window_frame_host_hwnd = nullptr;
	window_frame_host_proc = nullptr;
	window_frame_bound = false;
	//
	STARTUPINFO info;
	PROCESS_INFORMATION pinfo;
	ZeroMemory(&info, sizeof(STARTUPINFO));
	info.dwFlags = STARTF_USESHOWWINDOW;
	info.wShowWindow = SW_HIDE;
	info.cb = sizeof(STARTUPINFO);
	//
	if (CreateProcess(
		L"window_frame_host.exe", nullptr,
		nullptr, nullptr, false, 0,
		nullptr, nullptr,
		&info, &pinfo
	)) {
		window_frame_host_proc = pinfo.hProcess;
		// wait until the host-window loads up:
		WaitForInputIdle(pinfo.hProcess, INFINITE);
		// find the hwnd of window_frame_host:
		EnumWindows(&window_frame_enum_wnds, pinfo.dwThreadId);
	}
	//
	HWND fwnd = window_frame_host_hwnd;
	if (fwnd != nullptr) {
		// copy title:
		SetWindowText(fwnd, utf8.proc(title));
		// copy icons:
		SendMessage(fwnd, WM_SETICON, ICON_SMALL2, SendMessage(hwnd, WM_GETICON, ICON_SMALL2, 0));
		SendMessage(fwnd, WM_SETICON, ICON_SMALL, SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0));
		SendMessage(fwnd, WM_SETICON, ICON_BIG, SendMessage(hwnd, WM_GETICON, ICON_BIG, 0));
		// hook the game's wndproc so that it doesn't self-clear during resize:
		window_frame_wndproc_base = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC,
			(LONG_PTR)window_frame_wndproc_hook);
		//
		window_frame_set_rect_impl(x, y, w, h, true);
		//
		SetWindowLongPtr(fwnd, GWL_USERDATA, (LONG)hwnd);
		//
		window_frame_set_visible_impl(true, false);
		//
		return 1;
	} else return 0;
}

dllx double window_frame_cleanup_raw() {
	if (window_frame_host_hwnd != nullptr) {
		window_frame_set_visible_impl(false, false);
		SendMessage(window_frame_host_hwnd, WM_DESTROY, 0, 0);
	}
	return true;
}
