#include <SDKDDKVer.h>
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <cstdlib>
#include <stdio.h>

LRESULT CALLBACK wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return FALSE;
		case WM_ERASEBKGND:
			break;
			//return TRUE; // not needed here..?
		case WM_KEYDOWN: case WM_KEYUP: case WM_SETCURSOR: case WM_MOUSEMOVE:
		case WM_HOTKEY: case WM_CHAR: case WM_MENUCHAR: case WM_INITMENUPOPUP:
		case WM_SYSCHAR: case WM_SYSDEADCHAR: case WM_SYSKEYDOWN: case WM_SYSKEYUP:
			// forward keyboard events to game window:
			HWND game = (HWND)GetWindowLongPtr(hWnd, GWL_USERDATA);
			if (game != NULL) SendMessage(game, message, wParam, lParam);
			break;
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