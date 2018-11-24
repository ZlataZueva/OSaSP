#include "stdafx.h"
#include "Drawing.h"
#include "resource.h"
#include <vector>

using namespace std;

PAINTSTRUCT Drawing::ps;
HDC Drawing::hdc;
HBITMAP Drawing::hBmpBackground;
//BITMAP bmpBackground;
//HDC memDC;
HBITMAP Drawing::originalBmp;

BYTE Drawing::sizeMode = MEDIUM_SIZE;
INT Drawing::cellSize = MEDIUM_CELL_SIZE;
INT Drawing::radius = Drawing::cellSize / 3;
INT Drawing::penWidth = MEDIUM_PEN_WIDTH;
INT Drawing::fieldWidth = MEDIUM_FIELD_WIDTH;
INT Drawing::fieldHeight = MEDIUM_FIELD_HEIGHT;
INT Drawing::windowWidth = 0;
INT Drawing::windowHeight = 0;
BYTE Drawing::colorMode = NOTEBOOK_COLORS;
POINT Drawing::dotOver = POINT();
vector<vector<Drawing::PDOT> >Drawing::dots;
vector<vector<PPOINT> >Drawing::logicalToPhysical;

Drawing::Drawing()
{
}

VOID Drawing::DrawClosedAreas(HWND hWnd, vector<vector<UINT>> *closedAreas,vector<POINT> *pointsLogicalCoordinates, UINT playersAmount)
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (UINT i = 0; i < closedAreas->size(); i++)
	{
		INT player = (*closedAreas)[i][0] % playersAmount;
		SetPlayerColors(player);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		POINT* polygonPoints;
		polygonPoints = (POINT*)malloc(sizeof(POINT)*(closedAreas[i].size() - 1));
		for (UINT j = 0; j < closedAreas[i].size() - 1; j++)
		{
			POINT fromLogical =(*pointsLogicalCoordinates)[(*closedAreas)[i][j]];
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

VOID Drawing::DrawDots(UINT playersAmount)
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

VOID Drawing::FindPhysicalCoordinates(INT moveNum)
{
	if (moveNum == 0)
	{
		vector<vector<PPOINT>> physicalCoordinatesMatrix(fieldHeight);
		for (int i = 0; i < fieldHeight; i++)
		{
			vector<PPOINT> physicalCoordinatesArr(fieldWidth, (PPOINT)malloc(sizeof(POINT)));
			/*for (int j = 0; j < fieldWidth; j++)
			{
				physicalCoordinatesArr[j] = ;
			}*/
			physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
		}
		logicalToPhysical = physicalCoordinatesMatrix;
	}

	int marginLR = (windowWidth - fieldWidth*cellSize) / 2, marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	int y = marginTB;
	for (int i = 0; i<fieldHeight; i++)
	{
		int x = marginLR;
		for (int j = 0; j < fieldWidth; j++)
		{
			/*dots[i][j]->physicaÑoordinate.x = x;
			dots[i][j]->physicaÑoordinate.y = y;*/
			logicalToPhysical[i][j]->x = x;
			logicalToPhysical[i][j]->y = y;
			x += cellSize;
		}
		y += cellSize;
	}
}

VOID Drawing::FitSize(PRECT windowRect, INT moveNum)
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
	radius = cellSize / 3;
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

Drawing::PDOT Drawing::GetClosestDot(int x, int y)
{
	PDOT dotPtr = dots[0][0];
	if (Drawing::IsOnField(x, y))
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

POINT Drawing::GetClosestDotPos(int x, int y)
{
	if (IsOnField(x, y))
	{
		POINT dot = dot = *(logicalToPhysical)[0][0];
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

VOID Drawing::HighliteDot(HWND hWnd,INT player)
{
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetPlayerColors(player);
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
	Ellipse(hdc, dotOver.x - radius, dotOver.y - radius, dotOver.x + radius, dotOver.y + radius);
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
	//ReleaseDC(hWnd, hdc);
}

VOID Drawing::InitializeDotsMatrix()
{
	std::vector<std::vector<PDOT>> dotsMatrix(Drawing::fieldHeight);
	//std::vector<std::vector<PPOINT>> physicalCoordinatesMatrix(Drawing::fieldHeight);
	for (int i = 0; i < Drawing::fieldHeight; i++)
	{
		std::vector<PDOT> dotsArr(Drawing::fieldWidth);
		//std::vector<PPOINT> physicalCoordinatesArr(Drawing::fieldWidth);
		for (int j = 0; j < Drawing::fieldWidth; j++)
		{
			dotsArr[j] = (PDOT)malloc(sizeof(DOT));
			dotsArr[j]->state = EMPTY_POS;
			dotsArr[j]->logicalCoordinate.x = i;
			dotsArr[j]->logicalCoordinate.y = j;

			//physicalCoordinatesArr[j] = (PPOINT)malloc(sizeof(POINT));
		}
		dotsMatrix[i] = dotsArr;
		//physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
	}
	dots = dotsMatrix;
	//logicalToPhysical = physicalCoordinatesMatrix;
}

BOOLEAN Drawing::IsOnField(INT x, INT y)
{
	INT marginLR = (windowWidth - fieldWidth*cellSize) / 2,
		marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	return (x > marginLR && x<(marginLR + fieldWidth*cellSize) && y>marginTB && y < (marginTB + fieldHeight*cellSize));
}

VOID Drawing::LineField()
{
	int marginLR = (windowWidth - fieldWidth*cellSize) / 2, marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(NULL_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	//SelectObject(hdc, );
	Rectangle(hdc, marginLR, marginTB, marginLR + fieldWidth*cellSize, marginTB + fieldHeight*cellSize);
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
		LineTo(hdc, x, y + fieldHeight*cellSize);
		x += cellSize;
		//countHorisontal++;
	}
	x = marginLR;		

	while (y <= (windowHeight - marginTB))
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x + fieldWidth*cellSize, y);
		y += cellSize;
		//countVertical++;
	}
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
}

BOOL Drawing::LoadBackgroundImage(HINSTANCE hInst)
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


VOID Drawing::SetPlayerColors(UINT player)
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

VOID Drawing::ShowBackground(HWND hWnd)
{
	HDC memDC = CreateCompatibleDC(hdc);
	originalBmp = (HBITMAP)SelectObject(memDC, hBmpBackground);
	BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, originalBmp);
	DeleteDC(memDC);
	//BitBlt(hdc, 0, 0, x, y, memDC, 0, 0, SRCCOPY);
}

Drawing::~Drawing()
{
}
