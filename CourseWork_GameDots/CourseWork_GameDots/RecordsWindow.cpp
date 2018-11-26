#include "stdafx.h"
#include "RecordsWindow.h"
#include "Resource.h"
#include "Drawing.h"
#include "SavingResults.h"

HINSTANCE RecordsWindow::hInst;
HWND RecordsWindow::hWnd;
HWND RecordsWindow::hWndParent;

RecordsWindow::RecordsWindow()
{
}

BOOL RecordsWindow::CreateRecordsWindow(HWND hWndParent, HINSTANCE hInstance)
{
	hInst = hInstance;
	RecordsWindow::hWndParent = hWndParent;
	hWnd = CreateWindowW(RECORDS_WINDOW_CLASS, RECORDS_WINDOW_NAME, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | DS_CENTER,
		CW_USEDEFAULT, 0, RECORDS_WINDOW_DEFAULT_WIDTH, RECORDS_WINDOW_DEFAULT_HEIGHT, hWndParent, NULL, hInstance, NULL);

	//AddShowRecordsBtn(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}
	return TRUE;
}

ATOM RecordsWindow::RegisteRecordsWindowClass(HINSTANCE hInstance)
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
	wcex.lpszMenuName = RECORDS_WINDOW_NAME;
	wcex.lpszClassName = RECORDS_WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

VOID OnPaint(HWND hWnd)
{
	Drawing::hdc = BeginPaint(hWnd, &Drawing::ps);
	Drawing::ShowBackground(Drawing::hBmpBackground);
	Drawing::ShowTopImage();
	Drawing::ShowRecords(SavingResults::GetStringsRecords());
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

VOID RecordsWindow::ShowRecordsWindow()
{
	RECT* windowRect = new RECT();
	GetWindowRect(hWnd, windowRect);
	Drawing::windowWidth = windowRect->right - windowRect->left;
	Drawing::windowHeight = windowRect->bottom - windowRect->top;
	delete windowRect;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

LRESULT CALLBACK RecordsWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	case WM_SIZE:
		OnSize(hWnd,lParam);
		break;
	case WM_CLOSE:
		SendMessage(hWndParent, WM_CHILDACTIVATE, 0, 0);
		ShowWindow(hWnd, SW_HIDE);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

RecordsWindow::~RecordsWindow()
{
}
