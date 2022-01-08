/// window_frame_process.cpp
/// @author YellowAfterlife

#include "stdafx.h"
#include "window_frame.h"

static yal_set<WPARAM> blocked_syscommands;
static yal_set<WPARAM> hooked_syscommands;

static HWND hwnd;
static HWND get_game_hwnd() {
	auto hg = GetWindowFramePair(hwnd);
	if (!hg) return NULL;
	if (GetWindowFramePair(hg) != hwnd) {
		PostQuitMessage(0);
		return NULL;
	}
	return hg;
}
static DWORD wait_thread_id;
static HANDLE wait_thread_h;
static int min_width, min_height, max_width, max_height;
static HBRUSH background_brush;

static bool hook_syscommands;
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	trace_winmsg("frame", message, wParam, lParam);
	switch (message) {
		case WM_GETMINMAXINFO: {
			MINMAXINFO* mxi = (MINMAXINFO*)lParam;
			if (min_width >= 0) mxi->ptMinTrackSize.x = min_width;
			if (min_height >= 0) mxi->ptMinTrackSize.y = min_height;
			if (max_width >= 0) mxi->ptMaxTrackSize.x = max_width;
			if (max_height >= 0) mxi->ptMaxTrackSize.y = max_height;
			return 0;
		};
		case WM_DESTROY: {
			PostQuitMessage(0);
			return FALSE;
		};
		case WM_ERASEBKGND: {
			if (background_brush == nullptr) break;
			HDC hdc = (HDC)wParam;
			RECT cr; GetClientRect(hWnd, &cr);
			auto old_brush = SelectObject(hdc, background_brush);
			FillRect(hdc, &cr, background_brush);
			SelectObject(hdc, old_brush);
			return TRUE; // not needed here..?
		};
		case WM_KEYDOWN: case WM_KEYUP: case WM_SETCURSOR: case WM_MOUSEMOVE:
		case WM_HOTKEY: case WM_CHAR: case WM_MENUCHAR: case WM_INITMENUPOPUP:
		case WM_SYSCHAR: case WM_SYSDEADCHAR: case WM_SYSKEYDOWN: case WM_SYSKEYUP:
		case WM_DEVICECHANGE: {
			// forward device events to the game window:
			auto game = get_game_hwnd();
			if (game) SendMessage(game, message, wParam, lParam);
			break;
		};
		case WM_SETFOCUS: {
			// allegedly no side effects https://itch.io/post/537218
			auto game = get_game_hwnd();
			if (game) SetFocus(game);
			return 0;
		};
		case WM_SYSCOMMAND: {
			if (!hook_syscommands) break;
			if (wParam == SC_KEYMENU) break; // don't touch the system menu!
			auto cmd = wParam & ~15;
			if (blocked_syscommands.contains(cmd)) return TRUE;
			if (hooked_syscommands.contains(cmd)) {
				auto game = get_game_hwnd();
				if (game) SendMessage(game, WM_WF_SYSCOMMAND, wParam, lParam);
				return TRUE;
			}
			//MessageBoxA(0, "command", "me", MB_OK);
			break;
		};
		case WM_WF_SYSCOMMAND: {
			auto _hook = hook_syscommands;
			hook_syscommands = false;
			//DefWindowProc(hWnd, WM_SYSCOMMAND, wParam, lParam);
			auto result = SendMessage(hWnd, WM_SYSCOMMAND, wParam, lParam);
			hook_syscommands = _hook;
			return result;
		};
		case WM_WF_CALL: {
			switch ((WindowFrameCall_t)wParam) {
				case WindowFrameCall_t::DisableSysCommand: blocked_syscommands.add(lParam); break;
				case WindowFrameCall_t::EnableSysCommand: blocked_syscommands.remove(lParam); break;
				case WindowFrameCall_t::IsSysCommandEnabled: return !blocked_syscommands.contains(lParam);

				case WindowFrameCall_t::HookSysCommand: hooked_syscommands.add(lParam); break;
				case WindowFrameCall_t::UnhookSysCommand: hooked_syscommands.remove(lParam); break;
				case WindowFrameCall_t::IsSysCommandHooked: return hooked_syscommands.contains(lParam);

				case WindowFrameCall_t::SetBackgroundColor: {
					if (background_brush) DeleteObject(background_brush);
					if (lParam >= 0 && lParam <= 0xFFFFFF) {
						background_brush = CreateSolidBrush(RGB(
							(lParam & 0xff),
							(lParam >> 8) & 0xff,
							(lParam >> 16) & 0xff
						));
					} else background_brush = NULL;
					RedrawWindow(hWnd, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
				} break;

				case WindowFrameCall_t::SetMinWidth:  min_width = lParam; break;
				case WindowFrameCall_t::SetMinHeight: min_height = lParam; break;
				case WindowFrameCall_t::SetMaxWidth:  max_width = lParam; break;
				case WindowFrameCall_t::SetMaxHeight: max_height = lParam; break;

				//case WindowFrameCall_t::SetStdOut: trace_handle_set((HANDLE)lParam);
			}
			return TRUE;
		};
	}
	auto result = DefWindowProc(hWnd, message, wParam, lParam);
	return result;
}

static DWORD WINAPI wait_for_exit(void* _) {
	for (;;) {
		Sleep(100);
		auto game_hwnd = GetWindowFramePair(hwnd);
		if (game_hwnd && GetWindowFramePair(game_hwnd) != hwnd) {
			PostQuitMessage(0); // ExitProcess
		}
	}
}

static void init() {
	blocked_syscommands.init();
	hooked_syscommands.init();
	min_width = 80;
	min_height = 60;
	max_width = -1;
	max_height = -1;
	background_brush = NULL;
	hook_syscommands = true;
}

extern "C" __declspec(dllexport) void frame_process() {
	init();
	SetProcessDPIAware();
	//
	auto hInstance = GetModuleHandle(NULL);
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof wcex);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = FrameWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"window_frame";
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);
	//
	hwnd = CreateWindowW(L"window_frame", L"(this is a helper program for window_frame.dll)",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 400, 240,
		nullptr, nullptr, hInstance, nullptr);
	if (!hwnd) return;
	ShowWindow(hwnd, SW_HIDE);
	//
	wait_thread_h = CreateThread(NULL, 0, wait_for_exit, NULL, 0, &wait_thread_id);
	//
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}