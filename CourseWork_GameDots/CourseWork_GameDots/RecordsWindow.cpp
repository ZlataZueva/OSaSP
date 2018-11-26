#include "stdafx.h"
#include "RecordsWindow.h"
#include "Resource.h"
#include "Drawing.h"
#include "SavingResults.h"

HINSTANCE RecordsWindow::hInst;

RecordsWindow::RecordsWindow()
{
}

BOOL RecordsWindow::InitInstance(HWND hWndParent, HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(RECORDS_WINDOW_CLASS, RECORDS_WINDOW_NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWndParent, NULL, hInstance, NULL);

	//AddShowRecordsBtn(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

ATOM RecordsWindow::MyRegisterClass(HINSTANCE hInstance)
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
	Drawing::ShowBackground();
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
		case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
		OnSize(hWnd,lParam);
		break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

RecordsWindow::~RecordsWindow()
{
}
