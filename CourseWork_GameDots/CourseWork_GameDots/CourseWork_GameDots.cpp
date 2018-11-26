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
#include "SavingResults.h"
#include "RecordsWindow.h"

#define MAX_LOADSTRING 100
#define BTN_SHOWRECORDS_TITLE L""

#define DEFAULT_WIDTH_DIFFERENCE 15
#define DEFAULT_HEIGHT_DIRRERENCE 58
#define ID_BTN_SHOWRECORDS 30001
#define ID_BTN_RULES 30002


using namespace std;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR wsProgramPath[MAX_PATH];
HWND hWndBtnRules;


typedef struct SIZESTRUCT{
	INT x;
	INT y;
}SIZESTRUCT;

//Drawing* drawing;
GameLogic* gameLogic;
//SavingResults* savingResults;

//std::vector<EDGE> edges;
//BOOLEAN gameStarted = FALSE;
//BYTE movePlayer = FIRST_PLAYER;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID				AddRulesBtn(HWND hWnd);
VOID				AddShowRecordsBtn(HWND hWnd);
INT					OnCreate(HWND hWnd);
VOID				OnFinishGame(HWND hWnd);
INT					OnLButtonDown(HWND hWnd, LPARAM lParam);
INT					OnMouseMove(HWND hWnd, LPARAM lParam);
VOID				OnNewGame(HWND hWnd);
INT					OnPaint(HWND hWnd);
INT					OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
BOOL				OnSizing(WPARAM wParam, LPARAM lParam);
VOID				OnShowRecords(HWND hWnd);
VOID				PlaceDot(INT x, INT y);
INT_PTR CALLBACK	Rules(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{	
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	GetModuleFileNameW(NULL, wsProgramPath, MAX_PATH);
	
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
    wcex.hCursor        = LoadCursor(nullptr, IDC_HAND);
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd)
   {
      return FALSE;
   }
   AddShowRecordsBtn(hWnd);
   AddRulesBtn(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

VOID AddRulesBtn(HWND hWnd)
{
	hWndBtnRules = CreateWindowW(L"BUTTON",
		BTN_SHOWRECORDS_TITLE,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_MULTILINE | BS_BITMAP,
		WINDOW_DEFAULT_WIDTH-BTN_MARGIN-BTN_RULES_WIDTH,WINDOW_DEFAULT_HEIGHT- BTN_MARGIN - BTN_RULES_HEIGHT, BTN_RULES_WIDTH, BTN_RULES_HEIGHT,
		hWnd, (HMENU)ID_BTN_RULES, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	HBITMAP hBmpRules = LoadBitmapW(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	SendMessageW(hWndBtnRules, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpRules);
}

VOID AddShowRecordsBtn(HWND hWnd)
{
	HWND hWndBtnShowRecords = CreateWindowW(L"BUTTON",
		BTN_SHOWRECORDS_TITLE,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_CENTER | BS_MULTILINE | BS_BITMAP,
		BTN_MARGIN, MIN_FIELD_HEIGHT*MIN_CELL_SIZE - BTN_MARGIN - BTN_SHOW_RECORDS_HEIGHT, BTN_SHOWRECORDS_WIDTH, BTN_SHOW_RECORDS_HEIGHT,
		hWnd, (HMENU)ID_BTN_SHOWRECORDS, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
	HBITMAP hBmpRecords = LoadBitmapW(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	SendMessageW(hWndBtnShowRecords, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpRecords);
}

VOID OnChildClosed(HWND hWnd)
{
	RECT* windowRect = new RECT();
	GetWindowRect(hWnd, windowRect);
	//Drawing::FitSize(windowRect, GameLogic::moveNum);
	Drawing::windowWidth = windowRect->right-windowRect->left-DEFAULT_WIDTH_DIFFERENCE;
	Drawing::windowHeight = windowRect->bottom-windowRect->top-DEFAULT_HEIGHT_DIRRERENCE;
	Drawing::FindPhysicalCoordinates(GameLogic::moveNum);
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	delete windowRect;
}

INT OnCreate(HWND hWnd)
{
	//drawing = new Drawing();
	//drawing->FindPhysicalCoordinates(0);
	gameLogic = new GameLogic();
	Drawing::CreateDotsMatrix();
	Drawing::CreatePhysicalCoordinatesMatrix();
	Drawing::InitializeLogFont();
	Drawing::LoadImages(hInst);
	vector<PWCHAR> playersNames;
	playersNames.push_back(DEFAULT_NAME1);
	playersNames.push_back(DEFAULT_NAME2);
	playersNames.push_back(DEFAULT_NAME3);
	playersNames.push_back(DEFAULT_NAME4);
	playersNames.push_back(DEFAULT_NAME5);
	SavingResults::playersNames = playersNames;

	RecordsWindow::RegisteRecordsWindowClass(hInst);
	if (!RecordsWindow::CreateRecordsWindow(hWnd, hInst))
	{
		return -1;
	}
	return 0;
}

VOID OnFinishGame(HWND hWnd)
{
	SavingResults::UpdateRecords(gameLogic->capturedDotsAmounts);
	SavingResults::SaveRecords();
	DestroyWindow(hWnd);
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

VOID OnNewGame(HWND hWnd)
{
	gameLogic->StartNewGame();
	Drawing::InitializeDotsMatrix();
	InvalidateRgn(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

INT OnPaint(HWND hWnd)
{
	if (GetUpdateRgn(hWnd, NULL, TRUE) != NULLREGION)
	{
		Drawing::hdc = BeginPaint(hWnd, &(Drawing::ps));
		Drawing::ShowBackground(Drawing::hBmpBackground);
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
		Drawing::ShowScores(SavingResults::playersNames, gameLogic->capturedDotsAmounts);
		EndPaint(hWnd, &(Drawing::ps));
	}
	return 0;
}

VOID OnSaveAsGame()
{
	//расположение (ряд и колонка) каждой вершины по номеру
	//замкнутые области (как список номеров вершин) в порядке добавления 
}

VOID OnSaveGame()
{
	SavingResults::GetSaveGameDirectoryPath(wsProgramPath);
}

INT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	Drawing::dotOver.x = 1000;
	Drawing::dotOver.y = 1000;
	if ((wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) && GameLogic::moveNum == 0)
	{
		RECT* windowRect = new RECT();
		GetWindowRect(hWnd, windowRect);
		Drawing::FitSize(windowRect, GameLogic::moveNum);
		delete windowRect;
		Drawing::InitializeDotsMatrix();
	}
	Drawing::windowWidth = LOWORD(lParam);
	Drawing::windowHeight = HIWORD(lParam);
	Drawing::FindPhysicalCoordinates(GameLogic::moveNum);
	INT btnRulesL = Drawing::windowWidth - BTN_MARGIN - BTN_RULES_WIDTH;
	INT btnRulesT = Drawing::windowHeight - BTN_MARGIN - BTN_RULES_HEIGHT;
	MoveWindow(hWndBtnRules,btnRulesL, btnRulesT,BTN_RULES_WIDTH, BTN_RULES_HEIGHT, TRUE);
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

VOID OnShowRecords(HWND hWnd)
{
	SavingResults::ReadRecordsTable();
	SavingResults::UpdateRecords(gameLogic->capturedDotsAmounts);
	RecordsWindow::ShowRecordsWindow();
	//HWND hWndBtnBack = 
	//InvalidateRect(hWnd, NULL, TRUE);
	//UpdateWindow(hWnd);
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
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_NEWGAME:
				OnNewGame(hWnd);
                break;
			case IDM_SAVEAS:
				OnSaveAsGame();
				break;
			case IDM_SAVEGAME:
				OnSaveGame();
				break;
			case IDM_FINISHGAME:
				OnFinishGame(hWnd);
				break;
			case ID_BTN_SHOWRECORDS:
				OnShowRecords(hWnd);
				break;
			case ID_BTN_RULES:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Rules);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
		return OnPaint(hWnd);
	case WM_SIZING:
		return OnSizing(wParam, lParam);
	case WM_SIZE:
		return OnSize(hWnd, wParam, lParam);
	case WM_CHILDACTIVATE:
		OnChildClosed(hWnd);
		break;
	case WM_MOUSEMOVE:
		return OnMouseMove(hWnd, lParam);
	case WM_LBUTTONDOWN:
		return OnLButtonDown(hWnd, lParam);
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

INT_PTR CALLBACK Rules(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_PAINT:
		Drawing::ShowRulesImages(hDlg);
		break;
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