// CourseWork_GameDots.cpp: определяет точку входа для приложения.
//
#include "Windowsx.h"
#include "stdafx.h"
#include "CourseWork_GameDots.h"
#include <vector>
#include "string.h"
#include <shellapi.h>
#include <string>
#include <algorithm>

#include "Drawing.h"
#include "GameLogic.h"

#define MAX_LOADSTRING 100

using namespace std;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
//WCHAR wsProgramPath[MAX_PATH];


typedef struct SIZESTRUCT{
	INT x;
	INT y;
}SIZESTRUCT;

//Drawing* drawing;
GameLogic* gameLogic;

//std::vector<EDGE> edges;
//BOOLEAN gameStarted = FALSE;
//BYTE movePlayer = FIRST_PLAYER;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT					OnCreate();
INT					OnLButtonDown(HWND hWnd, LPARAM lParam);
INT					OnMouseMove(HWND hWnd, LPARAM lParam);
INT					OnPaint(HWND hWnd);
INT					OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
BOOL				OnSizing(WPARAM wParam, LPARAM lParam);
VOID				PlaceDot(INT x, INT y);


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

INT OnCreate()
{
	//drawing = new Drawing();
	//drawing->FindPhysicalCoordinates(0);
	gameLogic = new GameLogic();
	if (Drawing::LoadBackgroundImage(hInst))
		return 0;
	return -1;
}

INT OnLButtonDown(HWND hWnd, LPARAM lParam)
{
	PlaceDot(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	InvalidateRgn(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	return 0;
}

INT OnMouseMove(HWND hWnd, LPARAM lParam)
{
	POINT dot = Drawing::dotOver;
	INT radius = Drawing::radius + Drawing::penWidth;
	HRGN prevDotRgn = CreateEllipticRgn(dot.x - radius, dot.y - radius, dot.x + radius, dot.y + radius);
	Drawing::dotOver = Drawing::GetClosestDotPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	HRGN newDotRgn = CreateEllipticRgn(Drawing::dotOver.x - radius, Drawing::dotOver.y - radius, Drawing::dotOver.x + radius, Drawing::dotOver.y + radius);
	HRGN updateRgn = prevDotRgn;
	CombineRgn(updateRgn, prevDotRgn, newDotRgn, RGN_OR);
	InvalidateRgn(hWnd, updateRgn, TRUE);	
	UpdateWindow(hWnd);
	DeleteRgn(prevDotRgn);
	DeleteRgn(newDotRgn);
	DeleteRgn(updateRgn);
	return 0;
}

INT OnPaint(HWND hWnd)
{
	if (GetUpdateRgn(hWnd, NULL, TRUE) != NULLREGION)
	{
		Drawing::hdc = BeginPaint(hWnd, &(Drawing::ps));
		Drawing::ShowBackground();
		Drawing::LineField();
		Drawing::HighliteDot(GameLogic::moveNum % GameLogic::playersAmount, Drawing::dotOver);
		if (GameLogic::moveNum > 0)
			Drawing::DrawDots(GameLogic::playersAmount);
		if (GameLogic::moveNum > GameLogic::playersAmount*3)
		{
			vector<POINT> vertexesCoordinates;
			for (INT i = 0; i < gameLogic->vertexes.size(); i++)
			{
				vertexesCoordinates.push_back(gameLogic->vertexes[i].logicalCoordinate);
			}
			Drawing::DrawClosedAreas(&(gameLogic->closedAreas), &vertexesCoordinates, GameLogic::playersAmount);
		}
		EndPaint(hWnd, &(Drawing::ps));
	}
	return 0;
}

INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ((wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) && GameLogic::moveNum == 0)
	{
		RECT* windowRect = (RECT *)malloc(sizeof(RECT));
		GetWindowRect(hWnd, windowRect);
		Drawing::FitSize(windowRect, GameLogic::moveNum);
		Drawing::InitializeDotsMatrix();
	}
	Drawing::windowWidth = LOWORD(lParam);
	Drawing::windowHeight = HIWORD(lParam);
	Drawing::FindPhysicalCoordinates(GameLogic::moveNum);
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	return 0;
}

BOOL OnSizing(WPARAM wParam, LPARAM lParam)
{
	RECT* newWindowRect = (RECT *)(lParam);
	Drawing::FitSize(newWindowRect, GameLogic::moveNum);
	return TRUE;
}

VOID PlaceDot(INT x, INT y)
{
	if (Drawing::IsOnField(x,y))
	{
		Drawing::PDOT dotClicked = Drawing::GetClosestDot(x, y);
		BYTE state = dotClicked->state;
		if (state == EMPTY_POS)
		{
			dotClicked->state = (GameLogic::moveNum%GameLogic::playersAmount);
			gameLogic->AddVertex(GameLogic::moveNum, dotClicked->logicalCoordinate.x, dotClicked->logicalCoordinate.y, &(Drawing::logicalToPhysical));
			GameLogic::moveNum++;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		OnCreate();
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
		OnPaint(hWnd);
        break;
	case WM_SIZING:
		OnSizing(wParam, lParam);
		break;
	case WM_SIZE:
		OnSize(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(hWnd, lParam);
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, lParam);
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
