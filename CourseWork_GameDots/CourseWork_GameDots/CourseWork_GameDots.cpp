// CourseWork_GameDots.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "CourseWork_GameDots.h"
#include <vector>

#define MAX_LOADSTRING 100
#define MEDIUM_CELL_SIZE 20
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
#define NOTEBOOK_FIRST_DOT_PEN RGB(100, 0, 5)
#define NOTEBOOK_SECOND_DOT_PEN RGB(5,0,100)
#define NOTEBOOK_FIRST_DOT_BRUSH RGB(200, 0, 10)
#define NOTEBOOK_SECOND_DOT_BRUSH RGB(10,0,200)


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
PAINTSTRUCT ps;
HDC hdc;

typedef struct DOT {
	char state;
	POINT position;
}DOT, *PDOT;

typedef struct RECTSIZE{
	int width;
	int height;
}RECTSIZE;

int cellSize = MEDIUM_CELL_SIZE, radius = cellSize/4;
std::vector<std::vector<PDOT>> dots;
//std::vector<std::vector<POINT>> dotsPositions;
int windowWidth = 0, windowHeight = 0;
char colorMode = NOTEBOOK_COLORS;
bool gameStarted = false;
POINT dotOver;
char move = FIRST_PLAYER;

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

    // TODO: разместите код здесь.

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



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COURSEWORK_GAMEDOTS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COURSEWORK_GAMEDOTS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

void LineField(int fieldWidth, int fieldHeight)
{
	int marginLR = (windowWidth - fieldWidth)/2, marginTB = (windowHeight-fieldHeight)/2;
	//int fieldWidth = windowWidth - 2 * margin, fieldHeight = windowHeight - 2 * margin;
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
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
	//Rectangle(hdc, marginLR, marginTB, marginLR + fieldWidth, marginTB+fieldHeight);
	int countHorisontal = 0, countVertical = 0, x = marginLR, y=marginTB;
	while (x <= (windowWidth - marginLR))
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x, y + fieldHeight);
		x += cellSize;
		countHorisontal++;
	}
	x = marginLR;
	while (y <= (windowHeight - marginTB))
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x + fieldWidth, y);
		y += cellSize;
		countVertical++;
	}

	if (!gameStarted)
	{
		y = marginTB;
		std::vector<std::vector<PDOT>> dotsMatrix(countVertical);
		//std::vector<std::vector<char>> dotsPosMatrix(countHorisontal);
		for (int i = 0; i < countVertical; i++)
		{
			x = marginLR;
			std::vector<PDOT> dotsArr(countHorisontal);
			for (int j = 0; j < countHorisontal; j++)
			{
				dotsArr[j] = (PDOT) malloc (sizeof (DOT));
				dotsArr[j]->state = EMPTY_POS;
				dotsArr[j]->position.x = x;
				dotsArr[j]->position.y = y;
				x += cellSize;
			}
			dotsMatrix[i] = dotsArr;
			y += cellSize;
		}
		dots = dotsMatrix;
	}
	SelectObject(hdc, originalPen);
	SelectObject(hdc, originalBrush);
}

POINT GetClosestDotPos(int x, int y)
{
	POINT dot;
	if ((x < windowWidth) && (y < windowHeight))
	{
		dot = dots[0][0]->position;
		while (abs(dot.x - x) >= cellSize/2+1)
		{
			dot.x += cellSize;
		}
		while (abs(dot.y - y) >= cellSize/2+1)
		{
			dot.y += cellSize;
		}
	}
	return dot;
}

void HighliteDot(POINT dot)
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
	POINT dotPos = GetClosestDotPos(x, y);
	PDOT dotPtr = dots[0][0];
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
	return dotPtr;
}

char GetDotState(int x, int y)
{
	return GetClosestDot(x, y)->state;
}

void PlaceDot(int x, int y)
{
	gameStarted = true;
	PDOT dotClicked = GetClosestDot(x, y);
	char state = dotClicked->state;
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

void DrawDots()
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SelectObject(hdc, GetStockObject(NULL_PEN));
	int rowNum = (int)dots.size();
	int columnNum = (int)dots[0].size();

	switch (colorMode)
	{
	case STANDARD_COLORS:
	{
		SelectObject(hdc, GetStockObject(STANDARD_FIRST_DOT_BRUSH));
	}
	break;
	case NOTEBOOK_COLORS:
	{
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
		SelectObject(hdc, GetStockObject(STANDARD_SECOND_DOT_BRUSH));
	}
	break;
	case NOTEBOOK_COLORS:
	{
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
			LineField(windowWidth -10, windowHeight - 10);
			HighliteDot(dotOver);
			DrawDots();
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE:
		{
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
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
