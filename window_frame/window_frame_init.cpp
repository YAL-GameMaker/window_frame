#include "stdafx.h"
#include "window_frame.h"

static BOOL CALLBACK find_frame_hwnd(HWND hwnd, LPARAM param) {
	DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
	if (thread == (DWORD)param) {
		frame_hwnd = hwnd;
		return FALSE;
	} else return TRUE;
}

static bool hasFocus;
/// ->bool
dllx double window_frame_has_focus() {
	return hasFocus;
}

WNDPROC wndproc_base;
LRESULT wndproc_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	trace_winmsg("game", msg, wParam, lParam);
	switch (msg) {
		case WM_ACTIVATE:
			hasFocus = (wParam & 0xFFFF) != 0;
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_WF_SYSCOMMAND:
			queued_syscommands.add(wParam);
			return TRUE;
		case WM_SYSCOMMAND:
			if (!game_hook_syscommands) break;
			if (wParam == SC_KEYMENU) break; // don't touch the system menu!
			auto cmd = wParam & ~15;
			if (game_blocked_syscommands.contains(cmd)) return TRUE;
			if (game_hooked_syscommands.contains(cmd)) {
				queued_syscommands.add(wParam);
				return TRUE;
			}
			break;
	}
	return CallWindowProc(wndproc_base, hwnd, msg, wParam, lParam);
}


dllg bool window_frame_init(GAME_HWND hwnd, int x, int y, int w, int h, const char* title) {
	game_hwnd = hwnd;
	frame_hwnd = NULL;

	// https://stackoverflow.com/a/6924332/5578773
	HMODULE dllModule;
	if (GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCWSTR)&window_frame_init, &dllModule
		) == 0) {
		trace("GetModuleHandleEx failed, error %d", GetLastError());
		return false;
	}
	wchar_t dllPath[MAX_PATH];
	if (GetModuleFileNameW(dllModule, dllPath, std::size(dllPath)) == 0) {
		trace("GetModuleFileName failed, error %d", GetLastError());
		return false;
	}
	//tracew(L"path: <%s>", dllPath);

	WCHAR commandLine[1024];
	if (GetEnvironmentVariableW(L"windir", commandLine, std::size(commandLine)) == 0) {
		trace("GetEnvironmentVariable failed, error %d", GetLastError());
		return false;
	}
	yal_strcat(commandLine, L"\\system32\\rundll32.exe \"");
	yal_strcat(commandLine, dllPath);
	yal_strcat(commandLine, L"\" frame_process");
	//tracew(L"[%s]", commandLine);

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof si);//yal_memset(&si, 0, sizeof si);
	si.cb = sizeof si;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof pi);//yal_memset(&pi, 0, sizeof pi);

	if (!CreateProcessW(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		trace("CreateProcess failed, error %d", GetLastError());
		return false;
	}

	// wait for the frame to load up:
	WaitForInputIdle(pi.hProcess, INFINITE);

	// find the frame hwnd:
	EnumWindows(&find_frame_hwnd, pi.dwThreadId);
	if (frame_hwnd == NULL) {
		trace("Couldn't find the frame hwnd!");
		return false;
	}

	// we don't need these anymore:
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// match the frame to game:
	window_frame_set_caption(title);
	window_frame_sync_icons();

	//
	wndproc_base = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wndproc_hook);

	// link the two:
	WindowFrameCall(WindowFrameCall_t::SetStdOut, (LPARAM)GetStdHandle(STD_OUTPUT_HANDLE));
	SetWindowFramePair(frame_hwnd, game_hwnd);
	SetWindowFramePair(game_hwnd, frame_hwnd);
	frame_bound = true;
	window_frame_set_rect_impl(x, y, w, h, true);
	frame_bound = false;
	window_frame_set_visible_impl(true, false);

	return true;
}

static void init() {
	hasFocus = false;
	queued_syscommands.init();
	game_blocked_syscommands.init();
	game_hooked_syscommands.init();
	game_hook_syscommands = true;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			init();
			break;
		case DLL_PROCESS_DETACH:
			//
			break;
	}
	return TRUE;
}