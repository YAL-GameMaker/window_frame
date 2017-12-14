#include <SDKDDKVer.h>
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <cstdlib>
#include <stdio.h>
#include <map>
#include "../../window_frame/window_frame_interop.h"

#define SetWindowTextAF(hwnd, ...) { char b[256]; sprintf(b, __VA_ARGS__); SetWindowTextA(hwnd, b); }

//
bool window_command_direct = false;
std::map<WPARAM, bool> window_command_hooks;
std::map<WPARAM, bool> window_command_blocks;
//
int minWidth = 32;
int minHeight = 32;
int maxWidth = -1;
int maxHeight = -1;
HBRUSH background = nullptr;

LRESULT CALLBACK wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_GETMINMAXINFO: {
			MINMAXINFO* mxi = (MINMAXINFO*)lParam;
			if (minWidth >= 0) mxi->ptMinTrackSize.x = minWidth;
			if (minHeight >= 0) mxi->ptMinTrackSize.y = minHeight;
			if (minWidth >= 0) mxi->ptMinTrackSize.x = minWidth;
			if (minHeight >= 0) mxi->ptMinTrackSize.y = minHeight;
			return 0;
		};
		case WM_DESTROY: {
			PostQuitMessage(0);
			return FALSE;
		};
		case WM_ERASEBKGND: {
			if (background == nullptr) break;
			HDC hdc = (HDC)wParam;
			RECT cr; GetClientRect(hWnd, &cr);
			SelectObject(hdc, background);
			FillRect(hdc, &cr, background);
			return TRUE;
			//return TRUE; // not needed here..?
		};
		case WM_KEYDOWN: case WM_KEYUP: case WM_SETCURSOR: case WM_MOUSEMOVE:
		case WM_HOTKEY: case WM_CHAR: case WM_MENUCHAR: case WM_INITMENUPOPUP:
		case WM_SYSCHAR: case WM_SYSDEADCHAR: case WM_SYSKEYDOWN: case WM_SYSKEYUP: {
			// forward keyboard events to game window:
			HWND game = (HWND)GetWindowLongPtr(hWnd, GWL_USERDATA);
			if (game != nullptr) {
				if (IsWindow(game)) {
					SendMessage(game, message, wParam, lParam);
				} else PostQuitMessage(0);
			}
			break;
		};
		case WM_SYSCOMMAND: {
			if (window_command_direct) break;
			auto cmd = wParam & ~15;
			if (window_command_blocks.find(cmd) != window_command_blocks.end()) return TRUE;
			auto q = window_command_hooks.find(cmd);
			if (q != window_command_hooks.end()) {
				HWND game = (HWND)GetWindowLongPtr(hWnd, GWL_USERDATA);
				if (game != nullptr) {
					if (IsWindow(game)) {
						SendMessage(game, WFI_WM_CLIENT_NOTIFY_HOOK, wParam, lParam);
					} else PostQuitMessage(0);
				}
				window_command_hooks[q->first] = true;
				return TRUE;
			}
			break;
		};
		case WFI_WM_HOST_EXEC_SYSCOMMAND: { // dispatch hook
			window_command_direct = true;
			SendMessage(hWnd, WM_SYSCOMMAND, wParam, lParam);
			window_command_direct = false;
			return TRUE;
		};
		case WM_APP + 2: { // block/unblock
			switch (wParam) {
				case WFI_HOST_CONF_BLOCK_SYSCOMMAND: window_command_blocks[lParam] = true; break;
				case WFI_HOST_CONF_ALLOW_SYSCOMMAND: window_command_blocks.erase(lParam); break;
				case WFI_HOST_CONF_HOOK_SYSCOMMAND: window_command_hooks[lParam] = false; break;
				case WFI_HOST_CONF_UNHOOK_SYSCOMMAND: window_command_hooks.erase(lParam); break;
				case WFI_HOST_CONF_SET_BACKGROUND:
					if (background != nullptr) DeleteObject(background);
					if (lParam >= 0 && lParam <= 0xFFFFFF) {
						background = CreateSolidBrush(RGB(
							(lParam & 0xff),
							(lParam >> 8) & 0xff,
							(lParam >> 16) & 0xff
						));
					} else background = nullptr;
					break;
				case WFI_HOST_CONF_SET_MIN_WIDTH: minWidth = lParam; break;
				case WFI_HOST_CONF_SET_MIN_HEIGHT: minHeight = lParam; break;
				case WFI_HOST_CONF_SET_MAX_WIDTH: maxWidth = lParam; break;
				case WFI_HOST_CONF_SET_MAX_HEIGHT: maxHeight = lParam; break;
			}
			return TRUE;
		};
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
) {
	//
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndproc;
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
	HWND hWnd = CreateWindowW(L"window_frame", L"(this is a helper program for window_frame.dll)",
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 400, 240,
		nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, nCmdShow);
	//
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}