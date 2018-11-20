// CourseWork_GameDots.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "CourseWork_GameDots.h"
#include <vector>
#include "string.h"
#include <shellapi.h>

#define MAX_LOADSTRING 100
#define MEDIUM_CELL_SIZE 20
#define MEDIUM_FIELD_WIDTH 40
#define MEDIUM_FIELD_HEIGHT 25
#define EMPTY_POS 0
#define FIRST_PLAYER 1
#define SECOND_PLAYER 2
#define STANDARD_COLORS 0
#define STANDARD_PEN BLACK_PEN
#define STANDARD_BRUSH GRAY_BRUSH
#define STANDARD_FIRST_DOT_PEN WHITE_PEN
#define STANDARD_SECOND_DOT_PEN BLACK_PEN
#define STANDARD_FIRST_DOT_BRUSH LTGRAY_BRUSH
#define STANDARD_SECOND_DOT_BRUSH DKGRAY_BRUSH
#define NOTEBOOK_COLORS 1
#define NOTEBOOK_PEN RGB(0,0,200)
#define NOTEBOOK_BRUSH RGB(250,250,250)
#define NOTEBOOK_FIRST_DOT_PEN RGB(150, 10, 10)
#define NOTEBOOK_SECOND_DOT_PEN RGB(10,10,150)
#define NOTEBOOK_FIRST_DOT_BRUSH RGB(230, 0, 5)
#define NOTEBOOK_SECOND_DOT_BRUSH RGB(5,0,230)


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
//WCHAR wsProgramPath[MAX_PATH];
PAINTSTRUCT ps;
HDC hdc;
HBITMAP hBmpBackground;
BITMAP bmpBackground;
HDC memDC;

typedef struct DOT {
	BYTE state;
	POINT position;
}DOT, *PDOT;

typedef struct SIZESTRUCT{
	INT x;
	INT y;
}SIZESTRUCT;

INT cellSize = MEDIUM_CELL_SIZE, radius = cellSize/4;
INT fieldWidth = MEDIUM_FIELD_WIDTH, fieldHeight = MEDIUM_FIELD_HEIGHT;
std::vector<std::vector<PDOT>> dots;
INT windowWidth = 0, windowHeight = 0;
BYTE colorMode = NOTEBOOK_COLORS;
BOOLEAN gameStarted = FALSE;
POINT dotOver;
BYTE move = FIRST_PLAYER;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{	
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//GetModuleFileNameW(NULL, wsProgramPath, MAX_PATH);
	
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COURSEWORK_GAMEDOTS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COURSEWORK_GAMEDOTS));

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

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COURSEWORK_GAMEDOTS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

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

BOOL LoadBackgroundImage(HWND hWnd)
{
	/*WCHAR wsImagePath[MAX_PATH];
	wcscpy_s(wsImagePath, MAX_PATH, wsProgramPath);
	PWCHAR pwcLastSlash = wcsrchr(wsImagePath, L'\\');
	*pwcLastSlash = L'\0';
	pwcLastSlash = wcsrchr(wsImagePath, L'\\');
	wcscpy_s(pwcLastSlash, 20, L"\\images\\desktop.bmp");*/
	hBmpBackground = LoadBitmapW(hInst, MAKEINTRESOURCEW(IDB_BITMAP1));
	//hBmpBackground = (HBITMAP)LoadImageW(NULL, wsImagePath, IMAGE_BITMAP, windowWidth, windowHeight, LR_LOADFROMFILE | LR_LOADTRANSPARENT);
	GetObjectW(hBmpBackground, sizeof(bmpBackground), &bmpBackground);
	hdc = GetDC(hWnd);
	memDC = CreateCompatibleDC(hdc);
	SelectObject(memDC, hBmpBackground);
	ReleaseDC(hWnd, hdc);
	return TRUE;
}

VOID ShowBackground(int x, int y)
{
	BitBlt(hdc, 0, 0, x, y, memDC, 0, 0, SRCCOPY);
}

VOID LineField(int fieldWidth, int fieldHeight)
{
	int marginLR = (windowWidth - fieldWidth)/2, marginTB = (windowHeight-fieldHeight)/2;
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SelectObject(hdc, GetStockObject(NULL_PEN));
	Rectangle(hdc, marginLR, marginTB, marginLR + fieldWidth, marginTB + fieldHeight);
	SelectObject(hdc, GetStockObject(DC_PEN));
	switch (colorMode)
	{
	case STANDARD_COLORS:
		{
			SelectObject(hdc, GetStockObject(STANDARD_PEN));
			SelectObject(hdc, GetStockObject(STANDARD_BRUSH));
		}
		break;
	case NOTEBOOK_COLORS:
		{
			SetDCPenColor(hdc, NOTEBOOK_PEN);
			SetDCBrushColor(hdc, NOTEBOOK_BRUSH);
		}
		break;
	}
	//int countHorisontal = 0, countVertical = 0;
	int x = marginLR, y = marginTB;
	while (x <= (windowWidth - marginLR))
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x, y + fieldHeight);
		x += cellSize;
		//countHorisontal++;
	}
	x = marginLR;
	while (y <= (windowHeight - marginTB))
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x + fieldWidth, y);
		y += cellSize;
		//countVertical++;
	}
	SelectObject(hdc, originalPen);
	SelectObject(hdc, originalBrush);
}

VOID FindCoordinates()
{
	if (!gameStarted)
	{
		std::vector<std::vector<PDOT>> dotsMatrix(fieldHeight);
		for (int i = 0; i < fieldHeight; i++)
		{
			std::vector<PDOT> dotsArr(fieldWidth);
			for (int j = 0; j < fieldWidth; j++)
			{
				dotsArr[j] = (PDOT)malloc(sizeof(DOT));
				dotsArr[j]->state = EMPTY_POS;
			}
			dotsMatrix[i] = dotsArr;
		}
		dots = dotsMatrix;
	}

	int marginLR = (windowWidth - fieldWidth*cellSize) / 2, marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	int y = marginTB;
	for (int i =0; i<fieldHeight;i++)
	{
		int x = marginLR;
		for (int j = 0; j < fieldWidth; j++)
		{
			dots[i][j]->position.x = x;
			dots[i][j]->position.y = y;
			x += cellSize;
		}
		y += cellSize;
	}
}

BOOLEAN IsOnField(int x, int y)
{
	INT marginLR = (windowWidth - fieldWidth*cellSize) / 2,
		marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	return (x > marginLR && x<(marginLR + fieldWidth*cellSize) && y>marginTB && y < (marginTB + fieldHeight*cellSize));
}

POINT GetClosestDotPos(int x, int y)
{
	if (IsOnField(x, y))
	{
		POINT dot = dot = dots[0][0]->position;
		while (abs(dot.x - x) >= cellSize / 2 + 1)
		{
			dot.x += cellSize;
		}
		while (abs(dot.y - y) >= cellSize / 2 + 1)
		{
			dot.y += cellSize;
		}
		return dot;
	}
	else
		return dotOver;
}

VOID HighliteDot(POINT dot)
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	if (move == FIRST_PLAYER)
	{
		switch (colorMode)
		{
		case STANDARD_COLORS:
			{
				SelectObject(hdc, GetStockObject(STANDARD_FIRST_DOT_PEN));
			}
			break;
		case NOTEBOOK_COLORS:
			{
				SetDCPenColor(hdc, NOTEBOOK_FIRST_DOT_PEN);
			}
			break;
		}
	}
	else
	{
		switch (colorMode)
		{
		case STANDARD_COLORS:
		{
			SelectObject(hdc, GetStockObject(STANDARD_SECOND_DOT_PEN));
		}
		break;
		case NOTEBOOK_COLORS:
		{
			SetDCPenColor(hdc, NOTEBOOK_SECOND_DOT_PEN);
		}
		break;
		}
	}
	Ellipse(hdc, dot.x - radius, dot.y - radius, dot.x + radius, dot.y + radius);
	SelectObject(hdc, originalPen);
	SelectObject(hdc, originalBrush);
}

PDOT GetClosestDot(int x, int y)
{
	PDOT dotPtr = dots[0][0];
	if (IsOnField(x, y))
	{
		POINT dotPos = GetClosestDotPos(x, y);
		int rowNum = (int)dots.size();
		int columnNum = (int)dots[0].size();
		for (int i = 0; i < rowNum; i++)
		{
			for (int j = 0; j < columnNum; j++)
			{
				if (dots[i][j]->position.x == dotPos.x && dots[i][j]->position.y == dotPos.y)
				{
					dotPtr = dots[i][j];
				}
			}
		}
	}
	return dotPtr;
}

BYTE GetDotState(int x, int y)
{
	return GetClosestDot(x, y)->state;
}

VOID PlaceDot(int x, int y)
{
	
	if (IsOnField(x,y))
	{
		gameStarted = TRUE;
		PDOT dotClicked = GetClosestDot(x, y);
		BYTE state = dotClicked->state;
		if (state == EMPTY_POS)
		{
			switch (move)
			{
			case FIRST_PLAYER:
			{
				dotClicked->state = FIRST_PLAYER;
				move = SECOND_PLAYER;
			}
			break;
			case SECOND_PLAYER:
			{
				dotClicked->state = SECOND_PLAYER;
				move = FIRST_PLAYER;
			}
			break;
			}
		}
	}
}

BOOLEAN FindClosedArea()
{
	return FALSE;
} 

VOID DrawDots()
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	int rowNum = (int)dots.size();
	int columnNum = (int)dots[0].size();

	switch (colorMode)
	{
	case STANDARD_COLORS:
	{
		SelectObject(hdc, GetStockObject(STANDARD_FIRST_DOT_PEN));
		SelectObject(hdc, GetStockObject(STANDARD_FIRST_DOT_BRUSH));
	}
	break;
	case NOTEBOOK_COLORS:
	{
		SetDCPenColor(hdc, NOTEBOOK_FIRST_DOT_PEN);
		SetDCBrushColor(hdc, NOTEBOOK_FIRST_DOT_BRUSH);
	}
	break;
	}
	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			if (dots[i][j]->state == FIRST_PLAYER)
			{
				POINT dotPos = dots[i][j]->position;
				Ellipse(hdc, dotPos.x - radius, dotPos.y - radius, dotPos.x + radius, dotPos.y + radius);
			}
		}
	}

	switch (colorMode)
	{
	case STANDARD_COLORS:
	{
		SelectObject(hdc, GetStockObject(STANDARD_SECOND_DOT_PEN));
		SelectObject(hdc, GetStockObject(STANDARD_SECOND_DOT_BRUSH));
	}
	break;
	case NOTEBOOK_COLORS:
	{
		SetDCPenColor(hdc, NOTEBOOK_SECOND_DOT_PEN);
		SetDCBrushColor(hdc, NOTEBOOK_SECOND_DOT_BRUSH);
	}
	break;
	}
	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			if (dots[i][j]->state == SECOND_PLAYER)
			{
				POINT dotPos = dots[i][j]->position;
				Ellipse(hdc, dotPos.x - radius, dotPos.y - radius, dotPos.x + radius, dotPos.y + radius);
			}
		}
	}
	SelectObject(hdc, originalPen);
	SelectObject(hdc, originalBrush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			LoadBackgroundImage(hWnd);
		}
		break;
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
        }
        break;
    case WM_PAINT:
        { 
			hdc = BeginPaint(hWnd, &ps);
			ShowBackground(windowWidth, windowHeight);
			LineField(fieldWidth*cellSize, fieldHeight*cellSize);
			HighliteDot(dotOver);
			DrawDots();
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE:
		{
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
			FindCoordinates();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	case WM_MOUSEMOVE:
		{
			//hdc = BeginPaint(hWnd, &ps);
			HRGN rect1 = CreateRectRgn(dotOver.x - radius, dotOver.y - radius, dotOver.x + radius, dotOver.y + radius);
			dotOver = GetClosestDotPos(LOWORD(lParam), HIWORD(lParam));
			HRGN rect2 = CreateRectRgn(dotOver.x - radius, dotOver.y - radius, dotOver.x + radius, dotOver.y + radius);
			HRGN updateRgn = rect1;
			CombineRgn(updateRgn, rect1, rect2, RGN_OR);
			InvalidateRgn(hWnd, updateRgn, TRUE);
			UpdateWindow(hWnd);
			//EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			HRGN rect1 = CreateRectRgn(dotOver.x - radius, dotOver.y - radius, dotOver.x + radius, dotOver.y + radius);
			PlaceDot(LOWORD(lParam), HIWORD(lParam));
			HRGN rect2 = CreateRectRgn(LOWORD(lParam) - 2*radius, HIWORD(lParam) - 2*radius, LOWORD(lParam) + 2*radius, HIWORD(lParam) + 2*radius);
			HRGN updateRgn = rect2;
			CombineRgn(updateRgn, rect1, rect2, RGN_OR);
			InvalidateRgn(hWnd, updateRgn, TRUE);
			UpdateWindow(hWnd);
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
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
