#include "stdafx.h"
#include "window_frame.h"
yal_set<WPARAM> queued_syscommands;
yal_set<WPARAM> game_blocked_syscommands;
yal_set<WPARAM> game_hooked_syscommands;
bool game_hook_syscommands;
// Window Commands implementation for Window Frame

dllg double window_command_check(int button) {
	return queued_syscommands.remove(button);
}

dllg int window_command_run(int command, int lParam = 0) {
	if (frame_bound) {
		return SendMessageW(frame_hwnd, WM_WF_SYSCOMMAND, (WPARAM)command, (LPARAM)lParam);
	} else {
		auto _hook = game_hook_syscommands;
		game_hook_syscommands = false;
		auto result = SendMessageW(game_hwnd, WM_SYSCOMMAND, (WPARAM)command, (LPARAM)lParam);
		game_hook_syscommands = _hook;
		return result;
	}
}

dllg bool window_command_hook(int button) {
	WindowFrameCall(WindowFrameCall_t::HookSysCommand, button);
	game_hooked_syscommands.add(button);
	return true;
}

dllg bool window_command_unhook(int button) {
	WindowFrameCall(WindowFrameCall_t::UnhookSysCommand, button);
	game_hooked_syscommands.remove(button);
	return true;
}

dllg bool window_command_get_hooked(int button) {
	if (frame_bound) {
		return WindowFrameCall(WindowFrameCall_t::IsSysCommandHooked, button);
	} else {
		return game_hooked_syscommands.contains(button);
	}
}

dllg bool window_command_set_hooked(int button, bool hook) {
	if (hook) {
		return window_command_hook(button);
	} else return window_command_unhook(button);
}

/// Operation modes:
/// val=-1: returns current status
/// val>=0: changes status, returns whether successful
bool window_command_proc_active(int command, int val) {
	auto get = val < 0;
	auto setv = val > 0;
	switch (command) {
		case SC_MOVE: case SC_MOUSEMENU: {
			if (get) {
				if (frame_hwnd) {
					return WindowFrameCall(WindowFrameCall_t::IsSysCommandEnabled, command);
				} else {
					return !game_blocked_syscommands.contains(command);
				}
			} else if (setv) {
				game_blocked_syscommands.remove(command);
				return WindowFrameCall(WindowFrameCall_t::EnableSysCommand, command);
			} else {
				game_blocked_syscommands.add(command);
				return WindowFrameCall(WindowFrameCall_t::DisableSysCommand, command);
			}
		} break;
		case SC_CLOSE: {
			auto uid = (UINT)command;
			if (get) {
				auto menu = GetSystemMenu(frame_hwnd, false);
				return (GetMenuState(menu, uid, MF_BYCOMMAND) & MF_GRAYED) == 0;
			}
			game_blocked_syscommands.set(command, !setv);
			auto frame_menu = GetSystemMenu(frame_hwnd, false);
			return EnableMenuItem(frame_menu, uid, MF_BYCOMMAND | (setv ? MF_ENABLED : MF_GRAYED));
		} break;
		default: {
			long styleFlag = -1;
			switch (command) {
				case SC_SIZE: styleFlag = WS_THICKFRAME; break;
				case SC_MINIMIZE: styleFlag = WS_MINIMIZEBOX; break;
				case SC_MAXIMIZE: styleFlag = WS_MAXIMIZEBOX; break;
			}
			if (styleFlag < 0) return false;

			auto hwnd = frame_hwnd;
			auto styleBits = GetWindowLongW(hwnd, GWL_STYLE);
			if (get) {
				return (styleBits & styleFlag) == styleFlag;
			} else if (setv) {
				styleBits |= styleFlag;
			} else styleBits &= ~styleFlag;
			game_blocked_syscommands.set(command, !setv);
			SetWindowLongW(hwnd, GWL_STYLE, styleBits);
			return true;
		} break;
	}
	return false;
}

dllg bool window_command_get_active(int command) {
	return window_command_proc_active(command, -1);
}

dllg bool window_command_set_active(int command, bool val) {
	return window_command_proc_active(command, val ? 1 : 0);
}
