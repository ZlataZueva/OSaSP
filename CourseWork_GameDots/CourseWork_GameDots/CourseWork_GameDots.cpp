// CourseWork_GameDots.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "CourseWork_GameDots.h"
#include <vector>
#include "string.h"
#include <shellapi.h>
#include <string>
#include <algorithm>

#define MAX_LOADSTRING 100
#define MAX_CYCLESTRING 50
#define MAX_PLAYERS_AMOUNT 3
#define VERTEX_NEIGHBORS_AMOUNT 8
#define MIN_SIZE 0
#define MIN_CELL_SIZE 15
#define MIN_PEN_WIDTH 2
#define MIN_FIELD_WIDTH 30
#define MIN_FIELD_HEIGHT 20
#define MEDIUM_SIZE 1
#define MEDIUM_CELL_SIZE 20
#define MEDIUM_PEN_WIDTH 3
#define MEDIUM_FIELD_WIDTH 40
#define MEDIUM_FIELD_HEIGHT 25
#define MAX_SIZE 2
#define MAX_CELL_SIZE 25
#define MAX_PEN_WIDTH 5
#define MAX_FIELD_WIDTH 50
#define MAX_FIELD_HEIGHT 30
#define EMPTY_POS 255
#define FIRST_PLAYER 0
#define SECOND_PLAYER 1
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
#define NOTEBOOK_FIRST_DOT_PEN RGB(230, 5, 5)
#define NOTEBOOK_SECOND_DOT_PEN RGB(5,5,230)
#define NOTEBOOK_THIRD_DOT_PEN RGB(5,230,5)
#define NOTEBOOK_FIRST_DOT_BRUSH RGB(150, 10, 10)
#define NOTEBOOK_SECOND_DOT_BRUSH RGB(10,10,150)
#define NOTEBOOK_THIRD_DOT_BRUSH RGB(10,150,10)

using namespace std;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
//WCHAR wsProgramPath[MAX_PATH];
PAINTSTRUCT ps;
HDC hdc;
HBITMAP hBmpBackground;
//BITMAP bmpBackground;
//HDC memDC;
HBITMAP oldBmp;

typedef struct DOT {
	BYTE state;
	//POINT physicaСoordinate;
	POINT logicalCoordinate;
}DOT, *PDOT;

typedef struct SIZESTRUCT{
	INT x;
	INT y;
}SIZESTRUCT;

typedef struct VERTEX {
	INT num;
	POINT logicalCoordinate;
	//INT neighborsArr[VERTEX_NEIGHBORS_AMOUNT] = {EMPTY_POS};
	BOOLEAN isAvailable = TRUE;
}VERTEX, *PVERTEX;

typedef struct EDGE {
	INT num1;
	INT num2;
	BOOLEAN ofClosedArea = FALSE;
} EDGE;

BYTE sizeMode = MEDIUM_SIZE;
INT cellSize = MEDIUM_CELL_SIZE, radius = cellSize/3, penWidth = MEDIUM_PEN_WIDTH;
INT fieldWidth = MEDIUM_FIELD_WIDTH, fieldHeight = MEDIUM_FIELD_HEIGHT;
std::vector<std::vector<PPOINT>> logicalToPhysical;
std::vector<std::vector<PDOT>> dots;
std::vector<VERTEX> vertexes;
std::vector<std::vector<UINT>> closedAreas;
//std::vector<EDGE> edges;
INT windowWidth = 0, windowHeight = 0;
BYTE colorMode = NOTEBOOK_COLORS;
//BOOLEAN gameStarted = FALSE;
INT moveNum = 0;
INT playersAmount = 2;
POINT dotOver;
//BYTE movePlayer = FIRST_PLAYER;

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

VOID FitSize(PRECT windowRect)
{
	INT minWidthforMAX_SIZE = (MAX_FIELD_WIDTH)*MAX_CELL_SIZE;
	INT minHeightforMAX_SIZE = (MAX_FIELD_HEIGHT + 2)*MAX_CELL_SIZE;
	INT minWidthforMEDIUM_SIZE = (MEDIUM_FIELD_WIDTH + 1)*MEDIUM_CELL_SIZE;
	INT minHeightforMEDIUM_SIZE = (MEDIUM_FIELD_HEIGHT + 3)*MEDIUM_CELL_SIZE;
	INT minWidthforMIN_SIZE = (MIN_FIELD_WIDTH + 1)*MIN_CELL_SIZE;
	INT minHeightforMIN_SIZE = (MIN_FIELD_HEIGHT + 4)*MIN_CELL_SIZE;
	INT windowWidth = windowRect->right - windowRect->left;
	INT windowHeight = windowRect->bottom - windowRect->top;
	if (moveNum == 0)
	{
		if (windowWidth >= minWidthforMAX_SIZE && windowHeight >= minHeightforMAX_SIZE)
		{
			sizeMode = MAX_SIZE;
		}
		else if (windowWidth >= minWidthforMEDIUM_SIZE && windowHeight >= minHeightforMEDIUM_SIZE)
		{
			sizeMode = MEDIUM_SIZE;
		}
		else //if (windowWidth >= minWidthforMIN_SIZE && windowHeight >= minHeightforMIN_SIZE)
		{
			sizeMode = MIN_SIZE;
		}

		switch (sizeMode)
		{
		case MIN_SIZE:
			{
				cellSize = MIN_CELL_SIZE;
				fieldWidth = MIN_FIELD_WIDTH;
				fieldHeight = MIN_FIELD_HEIGHT;
				penWidth = MIN_PEN_WIDTH;
			}
			break;
		case MEDIUM_SIZE:
			{
				cellSize = MEDIUM_CELL_SIZE;
				fieldWidth = MEDIUM_FIELD_WIDTH;
				fieldHeight = MEDIUM_FIELD_HEIGHT;
				penWidth = MEDIUM_PEN_WIDTH;
			}
			break;
		case MAX_SIZE:
			{
				cellSize = MAX_CELL_SIZE;
				fieldWidth = MAX_FIELD_WIDTH;
				fieldHeight = MAX_FIELD_HEIGHT;
				penWidth = MAX_PEN_WIDTH;
			}
			break;
		}
	}
	INT minWidth = minWidthforMIN_SIZE, minHeight = minHeightforMIN_SIZE;
	switch (sizeMode)
	{
	case MEDIUM_SIZE:
		{
			minWidth = minWidthforMEDIUM_SIZE;
			minHeight = minHeightforMEDIUM_SIZE;
		}
		break;
	case MAX_SIZE:
		{
			minWidth = minWidthforMAX_SIZE;
			minHeight = minHeightforMAX_SIZE;
		}
		break;
	}
	if (windowWidth < minWidth)
	{
		windowRect->right = windowRect->left + minWidth;
	}
	if (windowHeight < minHeight)
	{
		windowRect->bottom = windowRect->top + minHeight;
	}
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
	//GetObjectW(hBmpBackground, sizeof(bmpBackground), &bmpBackground);
	/*hdc = GetDC(hWnd);
	
	ReleaseDC(hWnd, hdc);*/
	return TRUE;
}

VOID ShowBackground(HWND hWnd)
{
	HDC memDC = CreateCompatibleDC(hdc);
	oldBmp = (HBITMAP)SelectObject(memDC, hBmpBackground);
	BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, oldBmp);
	DeleteDC(memDC);
	//BitBlt(hdc, 0, 0, x, y, memDC, 0, 0, SRCCOPY);
}

VOID LineField(int fieldWidth, int fieldHeight)
{
	int marginLR = (windowWidth - fieldWidth)/2, marginTB = (windowHeight-fieldHeight)/2;
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(NULL_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	//SelectObject(hdc, );
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
	if (originalPen != NULL) 
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
}

VOID InitializeDotsMatrix()
{
	std::vector<std::vector<PDOT>> dotsMatrix(fieldHeight);
	std::vector<std::vector<PPOINT>> physicalCoordinatesMatrix(fieldHeight);
	for (int i = 0; i < fieldHeight; i++)
	{
		std::vector<PDOT> dotsArr(fieldWidth);
		std::vector<PPOINT> physicalCoordinatesArr(fieldWidth);
		for (int j = 0; j < fieldWidth; j++)
		{
			dotsArr[j] = (PDOT)malloc(sizeof(DOT));
			dotsArr[j]->state = EMPTY_POS;
			dotsArr[j]->logicalCoordinate.x = i;
			dotsArr[j]->logicalCoordinate.y = j;

			physicalCoordinatesArr[j] = (PPOINT)malloc(sizeof(POINT));
		}
		dotsMatrix[i] = dotsArr;
		physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
	}
	dots = dotsMatrix;
	logicalToPhysical = physicalCoordinatesMatrix;
}

VOID FindPhysicalCoordinates()
{
	if (moveNum == 0)
	{
		InitializeDotsMatrix();
	}

	int marginLR = (windowWidth - fieldWidth*cellSize) / 2, marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	int y = marginTB;
	for (int i =0; i<fieldHeight;i++)
	{
		int x = marginLR;
		for (int j = 0; j < fieldWidth; j++)
		{
			/*dots[i][j]->physicaСoordinate.x = x;
			dots[i][j]->physicaСoordinate.y = y;*/

			logicalToPhysical[i][j]->x = x;
			logicalToPhysical[i][j]->y = y;
			x += cellSize;
		}
		y += cellSize;
	}
}

VOID SetPlayerColors(UINT player);

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
		POINT dot = dot = *logicalToPhysical[0][0];
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

VOID HighliteDot(HWND hWnd, POINT dot)
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetPlayerColors(moveNum%playersAmount);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	/*switch (moveNum%playersAmount)
	{
	case FIRST_PLAYER:
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
		break;
	case SECOND_PLAYER:
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
		break;
	}*/
	Ellipse(hdc, dot.x - radius, dot.y - radius, dot.x + radius, dot.y + radius);
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
	//ReleaseDC(hWnd, hdc);
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
				if (logicalToPhysical[i][j]->x == dotPos.x && logicalToPhysical[i][j]->y == dotPos.y)
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

VOID FindCycles(PVERTEX lastPlaced, PVERTEX current, std::vector<UINT> *cycle, std::vector<std::vector<UINT>> *cyclesVector, UINT recursionDepth)
{
	//std::wstring newCycle = cycle + std::to_wstring(current->num);
	//PWCHAR strCurrentNum = (PWCHAR)malloc(sizeof(WCHAR) * 5);
	//_itow_s(current->num, strCurrentNum, sizeof(WCHAR) * 5, 10);
	//INT newCycleSize = wcslen(cycle) + wcslen(strCurrentNum)+2;
	//PWCHAR newCycle = (PWCHAR)malloc(sizeof(WCHAR)*newCycleSize);
	//wcscpy_s(newCycle, newCycleSize, cycle);
	//wcscat_s(newCycle, newCycleSize, strCurrentNum);
	////free(strCurrentNum);
	//strCurrentNum = NULL;
	std::vector<UINT> newCycle = *cycle;
	newCycle.push_back(current->num);

	current->isAvailable = FALSE;
	if (lastPlaced->num == current->num && recursionDepth > 2)
	{
		cyclesVector->push_back(newCycle);
	}
	else if (lastPlaced->num != current->num || recursionDepth == 0)
	{
		//wcscat_s(newCycle, newCycleSize, L"-");
		//newCycle += L"-";
		int player = moveNum%playersAmount;
		for (int i = player; i <= moveNum; i+=playersAmount)
		{
			if (i!=current->num && abs(vertexes[i].logicalCoordinate.x - current->logicalCoordinate.x) <= 1 &&        //если соседняя
				abs(vertexes[i].logicalCoordinate.y - current->logicalCoordinate.y) <= 1 &&
				(vertexes[i].isAvailable || vertexes[i].num == lastPlaced->num))                                     //если еще не добавлена в цикл или последняя
			{
				FindCycles(lastPlaced, &vertexes[i], &newCycle, cyclesVector, recursionDepth + 1);
			}
		}
		//free(newCycle);
		//newCycle = NULL;
		newCycle.clear();
	}
	else
	{
		/*free(newCycle);
		newCycle = NULL;*/
		newCycle.clear();
	}
	current->isAvailable = TRUE;
}

//int CompareUINT(const void *a, const void *b)
//{
//	return (*(UINT *)a - *(UINT *)b);
//}

BOOLEAN CompareVectors(std::vector<UINT> a, std::vector<UINT> b)
{
	/*std::vector<UINT> *vA = ((std::vector<UINT> *)a);
	std::vector<UINT> *vB = ((std::vector<UINT> *)b);*/
	if (a.size() > b.size())
		return TRUE;
	return FALSE;
}

std::vector<std::vector<UINT>> ExcludeIncludedInOther(std::vector<std::vector<UINT>> *cyclesFound)
{
	std::sort(cyclesFound->begin(), cyclesFound->end(), CompareVectors);
	std::vector<std::vector<UINT>> cyclesTempSorted = *cyclesFound;
	for (UINT i = 0; i < cyclesTempSorted.size(); i++)
	{
		std::sort(cyclesTempSorted[i].begin(), cyclesTempSorted[i].end());
	}
	std::vector<UINT> differentCyclesNums;
	differentCyclesNums.push_back(0);
	for (UINT i = 1; i < cyclesTempSorted.size(); i++)
	{
		BOOLEAN isToBeIncluded = TRUE;
		for (UINT j = 0; j < differentCyclesNums.size(); j++)
		{
			BOOLEAN isDifferent = TRUE;
			BOOLEAN isPartOfAnother = TRUE;
			std::vector<UINT> differentVector = cyclesTempSorted[differentCyclesNums[j]];
			if (cyclesTempSorted[i] == differentVector)
			{
				isDifferent = FALSE;
			}
			else
			{
				for (UINT num = 0; num < cyclesTempSorted[i].size(); num++)
				{
					if (!std::binary_search(differentVector.begin(), differentVector.end(), cyclesTempSorted[i][num]))
						isPartOfAnother = FALSE;
				}
			}
			if (!isDifferent || isPartOfAnother)
			{
				isToBeIncluded = FALSE;
				break;
			}
		}
		if (isToBeIncluded)
			differentCyclesNums.push_back(i);
	}
	std::vector<std::vector<UINT>> cyclesFiltered;
	for (UINT i = 0; i < differentCyclesNums.size(); i++)
		cyclesFiltered.push_back((*cyclesFound)[differentCyclesNums[i]]);
	return cyclesFiltered;
}

BOOLEAN FindOtherPlayersDots(vector<UINT> *cycle)
{
	BOOLEAN isFound = FALSE;
	INT player = vertexes[(*cycle)[0]].num%playersAmount;
	INT theMostLeft = fieldWidth, theMostRight = 0;
	INT theHighest = fieldHeight, theLowest = 0;
	POINT *polygonPoints;
	polygonPoints = (POINT*)malloc(sizeof(POINT)*(cycle->size() - 1));
	for (UINT i = 0; i < cycle->size() -1; i++)
	{
		POINT logicalCoordinate = vertexes[(*cycle)[i]].logicalCoordinate;
		/*INT y = vertexes[(*cycle)[i]].logicalCoordinate.y;
		INT x = vertexes[(*cycle)[i]].logicalCoordinate.x;*/
		if (logicalCoordinate.x > theMostRight)
			theMostRight = logicalCoordinate.x;
		if (logicalCoordinate.x < theMostLeft)
			theMostLeft = logicalCoordinate.x;
		if (logicalCoordinate.y < theHighest)
			theHighest = logicalCoordinate.y;
		if (logicalCoordinate.y > theLowest)
			theLowest = logicalCoordinate.y;
		
		POINT physicalCoordinate = *logicalToPhysical[logicalCoordinate.x][logicalCoordinate.y];
		polygonPoints[i] = physicalCoordinate;
	}
	HRGN polygonRgn = CreatePolygonRgn(polygonPoints, cycle->size() - 1, WINDING);
	/*for (INT column = theMostLeft+1; column < theMostRight; column++)
	{
		INT high = fieldWidth, low = 0;
		for (UINT i = 0; i < cycle->size() - 1; i++)
		{
			if (vertexes[(*cycle)[i]].logicalCoordinate.y == column)
			{
				INT x = vertexes[(*cycle)[i]].logicalCoordinate.x;
				if (x > low)
					low = x;
				if (x < high)
					high = x;
			}
		}*/
		for (UINT i = 0; i < vertexes.size(); i++)
		{
			if (vertexes[i].num%playersAmount != player && //если не своя
				vertexes[i].isAvailable &&				   //если не захвачена
				vertexes[i].logicalCoordinate.y > theHighest  && //если ниже самой верхней
				vertexes[i].logicalCoordinate.y < theLowest &&
				vertexes[i].logicalCoordinate.x>theMostLeft &&    //если правее левой границы
				vertexes[i].logicalCoordinate.x<theMostRight)     //если левее правой границы
			{
				POINT physicalCoordinate = *logicalToPhysical[vertexes[i].logicalCoordinate.x][vertexes[i].logicalCoordinate.y];
				HRGN pointRgn = CreateEllipticRgn(physicalCoordinate.x - 1, physicalCoordinate.y - 1, physicalCoordinate.x + 1, physicalCoordinate.y + 1);
				//HRGN combinedRgn = pointRgn;
				if (CombineRgn(polygonRgn, polygonRgn, pointRgn, RGN_AND) != NULLREGION)
				{
					vertexes[i].isAvailable = FALSE;
					isFound = TRUE;
				}
			}
		}
	//}
	return isFound;
}

BOOLEAN FindClosedArea()
{
	BOOLEAN isFound = FALSE;
	int player = moveNum%playersAmount;
	PVERTEX lastPlacedVertexPtr = &vertexes[moveNum];
	//std::vector<std::wstring> cycles;
	vector<vector<UINT>> cyclesFound;
	FindCycles(lastPlacedVertexPtr, lastPlacedVertexPtr, new vector<UINT>, &cyclesFound, 0);
	for (UINT i = 0; i < cyclesFound.size(); i++)
	{
		if (cyclesFound[i].size()<5)
		{
			cyclesFound.erase(cyclesFound.begin() + i);
		}
	}
	//for (int i = player; i <= moveNum; i += playersAmount)
	//	vertexes[i].isAvailable = TRUE;
	if (cyclesFound.size() > 0)
	{
		cyclesFound = ExcludeIncludedInOther(&cyclesFound);
		if (cyclesFound.size() > 0)
		{
			for (UINT i = 0; i < cyclesFound.size(); i++)
			{
				if (!FindOtherPlayersDots(&cyclesFound[i]))
				{
					cyclesFound.erase(cyclesFound.begin() + i);
				}
				else
				{
					closedAreas.push_back(cyclesFound[i]);
				}
			}
			if (cyclesFound.size() > 0)
			{
				isFound = TRUE;	
			}
		}
	}
	return isFound;
}

VOID SetPlayerColors(UINT player)
{
	switch (player)
	{
	case FIRST_PLAYER:
	{
		
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
			HPEN playersPen = CreatePen(PS_SOLID, penWidth, NOTEBOOK_FIRST_DOT_PEN);
			SelectObject(hdc, playersPen);
			SetDCBrushColor(hdc, NOTEBOOK_FIRST_DOT_BRUSH);
		}
		break;
		}
	}
	break;
	case SECOND_PLAYER:
	{
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
			HPEN playersPen = CreatePen(PS_SOLID, penWidth, NOTEBOOK_SECOND_DOT_PEN);
			SelectObject(hdc, playersPen);
			SetDCBrushColor(hdc, NOTEBOOK_SECOND_DOT_BRUSH);
		}
		break;
		}
	}
	break;
	}
}

VOID DrawClosedAreas(HWND hWnd)
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (UINT i = 0; i < closedAreas.size(); i++)
	{
		INT player = closedAreas[i][0] % playersAmount;
		SetPlayerColors(player);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		POINT* polygonPoints;
		polygonPoints = (POINT*)malloc(sizeof(POINT)*(closedAreas[i].size()-1));
		for (UINT j = 0; j < closedAreas[i].size()-1; j++)
		{
			POINT fromLogical = vertexes[closedAreas[i][j]].logicalCoordinate;
			//POINT toLogical = vertexes[closedAreas[i][j+1]].logicalCoordinate;
			POINT fromPhysical = *logicalToPhysical[fromLogical.x][fromLogical.y];
			//POINT toPhysical = *logicalToPhysical[toLogical.x][toLogical.y];
			polygonPoints[j] = fromPhysical;
			
			//HRGN polygon = CreatePolygonRgn(polygonPoints, closedAreas[i].size() - 1, WINDING);
			/*MoveToEx(hdc, fromPhysical->x, fromPhysical->y, NULL);
			LineTo(hdc, toPhysical->x, toPhysical->y);*/
		}
		Polygon(hdc, polygonPoints, closedAreas[i].size() - 1);
	}
	//
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
	ReleaseDC(hWnd, hdc);
}

VOID AddVertex(int num, int xLogical, int yLogical)
{
	VERTEX newVertex;
	newVertex.num = num;
	newVertex.logicalCoordinate.x = xLogical;
	newVertex.logicalCoordinate.y = yLogical;
	vertexes.push_back(newVertex);
	if (num > 5)
	{
		FindClosedArea();
	}
	//newVertex.isAvailable = TRUE;
	//for (int )
}

VOID PlaceDot(int x, int y)
{
	if (IsOnField(x,y))
	{
		PDOT dotClicked = GetClosestDot(x, y);
		BYTE state = dotClicked->state;
		if (state == EMPTY_POS)
		{
			dotClicked->state = (moveNum%playersAmount);
			AddVertex(moveNum, dotClicked->logicalCoordinate.x, dotClicked->logicalCoordinate.y);
			moveNum++;
		}
	}
}

VOID DrawDots()
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (UINT player = FIRST_PLAYER; player < FIRST_PLAYER + playersAmount; player++)
	{
		SetPlayerColors(player);
		/*for (UINT i = player; i < vertexes.size(); i += playersAmount)
		{
			POINT dotPos = *logicalToPhysical[vertexes[i].logicalCoordinate.x][vertexes[i].logicalCoordinate.y];
			Ellipse(hdc, dotPos.x - radius, dotPos.y - radius, dotPos.x + radius, dotPos.y + radius);
		}*/
		for (int i = 0; i < fieldHeight; i++)
		{
			for (int j = 0; j < fieldWidth; j++)
			{
				if (dots[i][j]->state == player)
				{
					POINT dotPos = *logicalToPhysical[i][j];
					Ellipse(hdc, dotPos.x - radius, dotPos.y - radius, dotPos.x + radius, dotPos.y + radius);
				}
			}
		}
	}
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
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
			ShowBackground(hWnd);
			LineField(fieldWidth*cellSize, fieldHeight*cellSize);
			HighliteDot(hWnd, dotOver);
			if (moveNum>0)
				DrawDots();
			if (moveNum > 5)
				DrawClosedAreas(hWnd);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZING:
	{
		RECT* newWindowRect = (RECT *)(lParam);
		FitSize(newWindowRect);
	}
	break;
	case WM_SIZE:
		{
			if ((wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) && moveNum == 0)
			{
				RECT* windowRect = (RECT *)malloc(sizeof(RECT));
				GetWindowRect(hWnd, windowRect);
				FitSize(windowRect);
			}
			windowWidth = LOWORD(lParam);
			windowHeight = HIWORD(lParam);
			FindPhysicalCoordinates();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	case WM_MOUSEMOVE:
		{
			//hdc = BeginPaint(hWnd, &ps);
			HRGN rect1 = CreateRectRgn(dotOver.x - (radius+3), dotOver.y - (radius+3), dotOver.x + (radius+3), dotOver.y + (radius+3));
			dotOver = GetClosestDotPos(LOWORD(lParam), HIWORD(lParam));
			HRGN rect2 = CreateRectRgn(dotOver.x - (radius + 3), dotOver.y - (radius + 3), dotOver.x + (radius + 3), dotOver.y + (radius + 3));
			HRGN updateRgn = rect1;
			CombineRgn(updateRgn, rect1, rect2, RGN_OR);
			InvalidateRgn(hWnd, updateRgn, TRUE);
			UpdateWindow(hWnd);
			//EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			PlaceDot(LOWORD(lParam), HIWORD(lParam));
			/*HRGN rect1 = CreateRectRgn(dotOver.x - radius, dotOver.y - radius, dotOver.x + radius, dotOver.y + radius);
			
			HRGN rect2 = CreateRectRgn(LOWORD(lParam) - 2*radius, HIWORD(lParam) - 2*radius, LOWORD(lParam) + 2*radius, HIWORD(lParam) + 2*radius);
			HRGN updateRgn = rect2;
			CombineRgn(updateRgn, rect1, rect2, RGN_OR);*/
			InvalidateRgn(hWnd, NULL, TRUE);
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
