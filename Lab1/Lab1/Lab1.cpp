// Lab1.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Lab1.h"
#include <windows.h>
#include <windef.h>
#include <Windowsx.h>

#define MAX_LOADSTRING 100
#define RECTANGLE 0
#define PICTURE 1

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

												// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HDC hdc, memDC;
int left = 30, right = 80, top = 40, bottom = 90;
const int move = 3;
bool autoMoveMode = false;
char autoMoveSide = 'l', spriteMode = RECTANGLE;
int autoMoveTimeout = 50, countTimers = 0;
HBITMAP pictureBitmap;
BITMAP bitmap;


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

void DrawSprite(HDC hdc, int left, int top, int right, int bottom, int mode = RECTANGLE)
{
	switch (mode)
	{
		case RECTANGLE:
		{
			SelectObject(hdc, GetStockObject(BLACK_PEN));
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, left, top, right, bottom);
			break;
		}
		case PICTURE:
		{
			right = left + bitmap.bmWidth;
			bottom = top + bitmap.bmHeight;
			BitBlt(hdc, left, top, right-left, bottom-top, memDC, 0, 0, SRCCOPY);
			break;
		}
	}
}

void MoveSprite(HWND hWnd, int moveX, int moveY)
{
	left += moveX;
	right += moveX;
	top += moveY;
	bottom += moveY;
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

void AutoMoveSprite(HWND hWnd, char &side)
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
		AutoMoveSprite(hWnd, side);
	}
	else
	{
		MoveSprite(hWnd, newLeft - left, newTop - top);
	}
}

char ShowChooseModeDialog(HWND hWnd)
{
	int choiseID = MessageBox(hWnd, L"Вы хотите видеть простой чёрный квадрат?\r\t Да - Чёрный квадрат\r\t Нет - Милая картинка", L"Выбор режима", MB_YESNO | MB_ICONQUESTION);
	if (choiseID == IDNO)
		return PICTURE;
	else
		return RECTANGLE;
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
					MessageBox(hWnd,
						L"Предмет: Операционные Системы и Системное Программирование\rТема: Изучение событийной архитекторы Windows-приложений, механизма обработки сообщений, механизма перерисовки окна\rВыполнил: Зуева З.А., гр.651001\rПроверил: Базылев Е.Н.",
						L"О программе",
						MB_OK | MB_ICONINFORMATION);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}

		case WM_CREATE:
		{
			spriteMode = ShowChooseModeDialog(hWnd);
			if (spriteMode == PICTURE)
			{
				pictureBitmap = (HBITMAP) LoadResource(NULL,FindResource(NULL,MAKEINTRESOURCE(IDB_BITMAP1), RT_BITMAP));
				GetObject(pictureBitmap, sizeof(bitmap), &bitmap); // Извлечение информации б объекте
				hdc = GetDC(hWnd); // Извлечение дескриптора дисплейного контекста устройства
				memDC = CreateCompatibleDC(hdc); // Создание виртуального контекста устройства, совместимого с заданным контекстом
				SelectObject(memDC, pictureBitmap); // Выбор объекта hBitmap в контекст устройства memBit
				ReleaseDC(hWnd, hdc); // Освобождение контекста устройства
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
			DrawSprite(hdc, left, top, right, bottom,spriteMode);
			EndPaint(hWnd, &ps);
			break;
		}

		case WM_DESTROY:
		{
			while (countTimers > 0)
			{
				KillTimer(hWnd, countTimers--);
			}
			PostQuitMessage(0);
			break;
		}

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
					autoMoveMode = false;
					pressedSide = 'l';
					break;
				}
				case VK_RIGHT:
				{
					autoMoveMode = false;
					pressedSide = 'r';
					break;
				}
				case VK_UP:
				{
					autoMoveMode = false;
					pressedSide = 'u';
					break;
				}
				case VK_DOWN:
				{
					autoMoveMode = false;
					pressedSide = 'd';
					break;
				}
				case VK_NUMPAD4:
				{
					autoMoveMode = true;
					pressedSide = 'l';
					break;
				}
				case VK_NUMPAD6:
				{
					autoMoveMode = true;
					pressedSide = 'r';
					break;
				}
				case VK_NUMPAD8:
				{
					autoMoveMode = true;
					pressedSide = 'u';
					break;
				}
				case VK_NUMPAD2:
				{
					autoMoveMode = true;
					pressedSide = 'd';
					break;
				}
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			autoMoveMode = false;
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				if (GetKeyState(VK_SHIFT) >= 0)
					MoveSprite(hWnd, 0, -move);
				else
					MoveSprite(hWnd, -move, 0);
			}
			else
			{
				if (GetKeyState(VK_SHIFT) >= 0)
					MoveSprite(hWnd, 0, move);
				else
					MoveSprite(hWnd, move, 0);
			}
			break;
		}

		case WM_TIMER:
		{
			if (autoMoveMode)
				AutoMoveSprite(hWnd, autoMoveSide);
			break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	if (autoMoveMode)
	{
		if (countTimers == 0)
		{
			SetTimer(hWnd, ++countTimers, autoMoveTimeout, NULL);
		}
		if (pressedSide == autoMoveSide && countTimers <= 8)
		{
			SetTimer(hWnd, ++countTimers, autoMoveTimeout, NULL);
		}
		else if (pressedSide != ' ')
		{
			autoMoveSide = pressedSide;
		}
	}
	else
	{
		while (countTimers > 1)
		{
			KillTimer(hWnd, countTimers--);
		}
		if (pressedSide != ' ')
		{
			AutoMoveSprite(hWnd, pressedSide);
		}
	}
	return 0;
}
