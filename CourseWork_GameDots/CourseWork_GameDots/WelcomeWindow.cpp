#include "stdafx.h"
#include "WelcomeWindow.h"
#include "resource.h"
#include "Drawing.h"
#include <vector>

using namespace std;

HINSTANCE WelcomeWindow::hInst;
HWND WelcomeWindow::hWnd;
HWND WelcomeWindow::hWndParent;
BYTE WelcomeWindow::playersCount = 0;
vector<PWCHAR> WelcomeWindow::nicknames(5);
vector<HWND> hWndEdits;
PWCHAR defaultNames[5]{ DEFAULT_NAME1,DEFAULT_NAME2,DEFAULT_NAME3,DEFAULT_NAME4,DEFAULT_NAME5 };

VOID AddEdit(HWND hWnd);

WelcomeWindow::WelcomeWindow()
{
}

BOOL WelcomeWindow::CreateWelcomeWindow(HWND hWndParent, HINSTANCE hInstance)
{
	hInst = hInstance;
	WelcomeWindow::hWndParent = hWndParent;
	hWnd = CreateWindowW(WELCOME_WINDOW_CLASS, WELCOME_WINDOW_NAME, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | DS_CENTER,
		200, 200, WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHTS, hWndParent, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	return TRUE;
}

VOID OnCreate(HWND hWnd)
{
	HWND hWndBtnStart = CreateWindowW(L"BUTTON",
		L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_MULTILINE | BS_BITMAP,
		(WELCOME_WINDOW_WIDTH - BTN_START_WIDTH)/2, WELCOME_WINDOW_HEIGHTS - BTN_MARGIN*4 - BTN_START_HEIGHT, BTN_START_WIDTH, BTN_START_HEIGHT,
		hWnd, (HMENU)ID_BTN_START, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	HBITMAP hBmpStart = LoadBitmapW(WelcomeWindow::hInst, MAKEINTRESOURCE(IDB_BITMAP8));
	SendMessageW(hWndBtnStart, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpStart);

	HWND hWndBtnAddPlayer = CreateWindowW(L"BUTTON",
		L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_MULTILINE | BS_BITMAP,
		WELCOME_WINDOW_WIDTH -(WELCOME_WINDOW_WIDTH - EDIT_NICKNAME_WIDTH)/2+BTN_MARGIN,
		EDIT_MARGIN, BTN_ADDPLAYER_WIDTH, BTN_ADDPLAYER_HEIGHT,
		hWnd, (HMENU)ID_BTN_ADDPLAYER, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	HBITMAP hBmpAddPlayer = LoadBitmapW(WelcomeWindow::hInst, MAKEINTRESOURCE(IDB_BITMAP9));
	SendMessageW(hWndBtnAddPlayer, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpAddPlayer);

	AddEdit(hWnd);
	AddEdit(hWnd);
}

VOID AddEdit(HWND hWnd)
{
	hWndEdits.push_back(
	 CreateWindowW(L"EDIT",
		L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_LEFT,
		(WELCOME_WINDOW_WIDTH - EDIT_NICKNAME_WIDTH) / 2, WelcomeWindow::playersCount*(EDIT_MARGIN+EDIT_NICKNAME_HEIGHT)+EDIT_MARGIN, EDIT_NICKNAME_WIDTH, EDIT_NICKNAME_HEIGHT,
		hWnd, 0, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL));
	SendMessageW(hWndEdits[WelcomeWindow::playersCount], EM_SETLIMITTEXT, MAX_NICKNAME-1, 0);
	SendMessageW(hWndEdits[WelcomeWindow::playersCount], WM_SETTEXT, 0,(LPARAM) defaultNames[WelcomeWindow::playersCount]);
	WelcomeWindow::playersCount++;
}

ATOM WelcomeWindow::RegisteWelcomeWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = WELCOME_WINDOW_NAME;
	wcex.lpszClassName = WELCOME_WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

VOID OnPaint(HWND hWnd)
{
	Drawing::hdc = BeginPaint(hWnd, &Drawing::ps);
	Drawing::ShowBackground(Drawing::hBmpBackground);
	//Drawing::ShowTopImage();
	//Drawing::ShowRecords(SavingResults::GetStringsRecords());
	//Drawing::LineField();
	EndPaint(hWnd, &(Drawing::ps));
}

VOID OnSize(HWND hWnd, LPARAM lParam)
{
	Drawing::windowWidth = LOWORD(lParam);
	Drawing::windowHeight = HIWORD(lParam);
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

VOID WelcomeWindow::ShowWelcomeWindow()
{
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	Drawing::windowWidth = windowRect.right - windowRect.left;
	Drawing::windowHeight = windowRect.bottom - windowRect.top;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

LRESULT CALLBACK WelcomeWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		OnCreate(hWnd);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case ID_BTN_START:
		{
			for (INT i = 0; i < playersCount; i++)
			{
				nicknames[i] = new WCHAR[MAX_NICKNAME];
				INT textLength = GetWindowTextLengthW(hWndEdits[i]);
				GetWindowTextW(hWndEdits[i], nicknames[i], MAX_NICKNAME);
			}
			while (nicknames.size() > playersCount)
				nicknames.pop_back();
			SendMessage(hWndParent, WM_CHILDACTIVATE, playersCount, playersCount);
			ShowWindow(hWnd, SW_HIDE);
		}
		break;
		case ID_BTN_ADDPLAYER:
		{
			if (playersCount < 5)
			{
				AddEdit(hWnd);

			}
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	case WM_SIZE:
		OnSize(hWnd, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

WelcomeWindow::~WelcomeWindow()
{
}
