/// @author YellowAfterlife

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>

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
	}
	return CallWindowProc(window_frame_wndproc_base, hwnd, msg, wp, lp);
}

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
		long ws = GetWindowLong(hwnd, GWL_STYLE);
		ws = ws & ~WS_POPUP | WS_CHILD;
		SetWindowLong(hwnd, GWL_STYLE, ws);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOOLWINDOW);
		SetParent(hwnd, window_frame_host_hwnd);
		MoveWindow(hwnd, 0, 0, w, h, TRUE);
		SetFocus(hwnd);
		//
		return 1;
	} else return 0;
}

dllx double window_frame_get_width() {
	RECT rect;
	return GetClientRect(window_frame_host_hwnd, &rect) ? (rect.right - rect.left) : 0;
}

dllx double window_frame_get_height() {
	RECT rect;
	return GetClientRect(window_frame_host_hwnd, &rect) ? (rect.bottom - rect.top) : 0;
}

dllx double window_frame_get_x() {
	RECT rect;
	return GetClientRect(window_frame_host_hwnd, &rect) ? rect.left : 0;
}

dllx double window_frame_get_y() {
	RECT rect;
	return GetClientRect(window_frame_host_hwnd, &rect) ? rect.top : 0;
}

dllx double window_frame_set_rect(double x, double y, double w, double h) {
	return window_frame_set_rect_impl((int)x, (int)y, (int)w, (int)h, false) != 0;
}

dllx double window_frame_set_caption(char* text) {
	return SetWindowText(window_frame_host_hwnd, utf8.proc(text));
}