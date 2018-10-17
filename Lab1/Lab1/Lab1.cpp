// Lab1.cpp: ���������� ����� ����� ��� ����������.
//

#include "stdafx.h"
#include "Lab1.h"
#include <windows.h>
#include <windef.h>
#include <Windowsx.h>

#define MAX_LOADSTRING 100

// ���������� ����������:
HINSTANCE hInst;                                // ������� ���������
WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����

												// ��������� ���������� �������, ���������� � ���� ������ ����:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int left = 30, right = 80, top = 40, bottom = 90;
const int move = 3;
HDC hdc;
bool autoMove = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ���������� ��� �����.

	// ������������� ���������� �����
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ��������� ������������� ����������:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	MSG msg;

	// ���� ��������� ���������:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  �������: MyRegisterClass()
//
//  ����������: ������������ ����� ����.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   �������: InitInstance(HINSTANCE, int)
//
//   ����������: ��������� ��������� ���������� � ������� ������� ����.
//
//   �����������:
//
//        � ������ ������� ���������� ���������� ����������� � ���������� ����������, � �����
//        ��������� � ��������� �� ����� ������� ���� ���������.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // ��������� ���������� ���������� � ���������� ����������

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void MoveRect(HWND hWnd, int moveX, int moveY)
{
	left += moveX;
	right += moveX;
	top += moveY;
	bottom += moveY;
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

//
//  �������: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����������:  ������������ ��������� � ������� ����.
//
//  WM_COMMAND � ���������� ���� ����������
//  WM_PAINT � ���������� ������� ����
//  WM_DESTROY � ��������� ��������� � ������ � ���������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// ��������� ����� � ����:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		/*SelectObject(hdc, GetStockObject(DC_PEN));
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, RGB(255, 0, 0));
		SetDCPenColor(hdc, RGB(0, 0, 255));*/
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(BLACK_BRUSH));
		Rectangle(hdc, left, top, right, bottom);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONUP:
	{
		HDC hdc = GetDC(hWnd);
		WORD xPos, yPos;
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		left = xPos - 25;
		right = xPos + 25;
		top = yPos + 25;
		bottom = yPos - 25;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		ReleaseDC(hWnd, hdc);
	}
	break;

	case WM_KEYDOWN:
	{

		switch (wParam)
		{
		case VK_LEFT:
		{
			MoveRect(hWnd, -move, 0);
			break;
		}
		case VK_RIGHT:
		{
			MoveRect(hWnd, move, 0);
			break;
		}
		case VK_UP:
		{
			MoveRect(hWnd, 0, -move);
			break;
		}
		case VK_DOWN:
		{
			MoveRect(hWnd, 0, move);
			break;
		}
		case VK_NUMPAD4:

		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			if (GetKeyState(VK_SHIFT) >= 0)
				MoveRect(hWnd, 0, -move);
			else
				MoveRect(hWnd, -move, 0);
		}
		else
		{
			if (GetKeyState(VK_SHIFT) >= 0)
				MoveRect(hWnd, 0, move);
			else
				MoveRect(hWnd, move, 0);
		}
		break;
	}

	case WM_MOUSEMOVE:
	{
		SetFocus(hWnd);
		break;
	}


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���������� ��������� ��� ���� "� ���������".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}