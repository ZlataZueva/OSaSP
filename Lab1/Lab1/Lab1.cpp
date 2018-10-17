// Lab1.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Lab1.h"
#include <windows.h>
#include <windef.h>
#include <Windowsx.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

												// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int left = 30, right = 80, top = 40, bottom = 90;
const int move = 3;
HDC hdc;
bool autoMove = false, timer = false;
char autoMoveSide = 'l';
int autoMoveTimeout = 20;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

	MSG msg;

	// Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

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

void AutoMoveRect(HWND hWnd, char &side)
{
	int newLeft=left, newRight=right, newTop=top, newBottom=bottom;
	char oppositeSide = ' ';
	switch (side)
	{
		case 'l':
		{
			newLeft -= move;
			newRight = -move;
			oppositeSide = 'r';
			break;
		}
		case 'r':
		{
			newLeft += move;
			newRight += move;
			oppositeSide = 'l';
			break;
		}
		case 'u':
		{
			newTop -= move;
			newBottom -= move;
			oppositeSide = 'd';
			break;
		}
		case 'd':
		{
			newTop += move;
			newBottom += move;
			oppositeSide = 'u';
			break;
		}
	}

	RECT window; int width = 0, height = 0;
	if (GetClientRect(hWnd, &window))
	{
		width = window.right - window.left;
		height = window.bottom - window.top;
	}

	if (newLeft<0 || newRight>width || newTop<0 || newBottom>height)
	{
		side = oppositeSide;
		AutoMoveRect(hWnd, side);
	}
	else
	{
		MoveRect(hWnd, newLeft - left, newTop - top);
	}
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	char pressedSide = ' ';
	switch (message)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Разобрать выбор в меню:
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
			break;
		}

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
			break;
		}

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_LEFT:
				{
					autoMove = false;
					MoveRect(hWnd, -move, 0);
					break;
				}
				case VK_RIGHT:
				{
					autoMove = false;
					MoveRect(hWnd, move, 0);
					break;
				}
				case VK_UP:
				{
					autoMove = false;
					MoveRect(hWnd, 0, -move);
					break;
				}
				case VK_DOWN:
				{
					autoMove = false;
					MoveRect(hWnd, 0, move);
					break;
				}
				case VK_NUMPAD4:
				{
					autoMove = true;
					pressedSide = 'l';
					break;
				}
				case VK_NUMPAD6:
				{
					autoMove = true;
					pressedSide = 'r';
					break;
				}
				case VK_NUMPAD8:
				{
					autoMove = true;
					pressedSide = 'u';
					break;
				}
				case VK_NUMPAD2:
				{
					autoMove = true;
					pressedSide = 'd';
					break;
				}
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			autoMove = false;
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

		case WM_TIMER:
		{
			if (autoMove)
				AutoMoveRect(hWnd, autoMoveSide);
			break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	if (autoMove)
	{
		if (!timer)
		{
			SetTimer(hWnd, 1, autoMoveTimeout, NULL);
			timer = true;
		}
		if (pressedSide == autoMoveSide)
		{
			if (autoMoveTimeout > 1)
			{
				autoMoveTimeout -= 1;
			}
		}
		else if (pressedSide != ' ')
		{
			autoMoveSide = pressedSide;
		}
	}
	else
	{
		if (timer)
		{
			KillTimer(hWnd, 1);
			timer = false;
		}
	}

	return 0;
}

// Обработчик сообщений для окна "О программе".
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
