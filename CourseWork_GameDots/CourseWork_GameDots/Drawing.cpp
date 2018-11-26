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
HPEN playersPen;
LOGFONT Drawing::logFont;
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

VOID Drawing::CreateDotsMatrix()
{
	std::vector<std::vector<PDOT>> dotsMatrix(MAX_FIELD_HEIGHT);
	//std::vector<std::vector<PPOINT>> physicalCoordinatesMatrix(Drawing::fieldHeight);
	for (int i = 0; i < MAX_FIELD_HEIGHT; i++)
	{
		std::vector<PDOT> dotsArr(MAX_FIELD_WIDTH);
		//std::vector<PPOINT> physicalCoordinatesArr(Drawing::fieldWidth);
		for (int j = 0; j < MAX_FIELD_WIDTH; j++)
		{
			dotsArr[j] = new DOT();
			//physicalCoordinatesArr[j] = (PPOINT)malloc(sizeof(POINT));
		}
		dotsMatrix[i] = dotsArr;
		//physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
	}
	dots = dotsMatrix;
}

VOID Drawing::CreatePhysicalCoordinatesMatrix()
{
	vector<vector<PPOINT>> physicalCoordinatesMatrix(MAX_FIELD_HEIGHT);
	for (int i = 0; i < MAX_FIELD_HEIGHT; i++)
	{
		vector<PPOINT> physicalCoordinatesArr(MAX_FIELD_WIDTH);
		for (int j = 0; j < MAX_FIELD_WIDTH; j++)
		{
			physicalCoordinatesArr[j] = new POINT();//(PPOINT)malloc(sizeof(POINT));
		}
		physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
	}
	logicalToPhysical = physicalCoordinatesMatrix;
}

//VOID DeleteDotsMatrix()
//{
//	for (int i = 0; i < MAX_FIELD_HEIGHT; i++)
//	{
//		for (int j = 0; j < MAX_FIELD_WIDTH; j++)
//		{
//			delete dots[i][j];
//		}
//	}
//}
//
//VOID DeletePhysicalCoordinatesMatrix()
//{
//	for (int i = 0; i < MAX_FIELD_HEIGHT; i++)
//	{
//		for (int j = 0; j < MAX_FIELD_WIDTH; j++)
//		{
//			delete logicalToPhysical[i][j];
//		}
//	}
//}

VOID Drawing::DrawClosedAreas(vector<vector<UINT>> *closedAreas,vector<POINT> *pointsLogicalCoordinates, UINT playersAmount)
{
	//HDC hdc = GetDC(hWnd);
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (UINT i = 0; i < closedAreas->size(); i++)
	{
		INT player = (*closedAreas)[i][0] % playersAmount;
		SetPlayerColors(hdc, player);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		POINT* polygonPoints = new POINT[(*closedAreas)[i].size()-1];// = (POINT*)malloc(sizeof(POINT)*(closedAreas[i].size()));
		for (UINT j = 0; j < (*closedAreas)[i].size() - 1; j++)
		{
			POINT logicalCoordinate =(*pointsLogicalCoordinates)[(*closedAreas)[i][j]];
			//POINT toLogical = vertexes[closedAreas[i][j+1]].logicalCoordinate;
			POINT physicalCoordinate = *logicalToPhysical[logicalCoordinate.x][logicalCoordinate.y];
			//POINT toPhysical = *logicalToPhysical[toLogical.x][toLogical.y];
			polygonPoints[j] = physicalCoordinate;
			//HRGN polygon = CreatePolygonRgn(polygonPoints, closedAreas[i].size() - 1, WINDING);
			/*MoveToEx(hdc, fromPhysical->x, fromPhysical->y, NULL);
			LineTo(hdc, toPhysical->x, toPhysical->y);*/
		}
		Polygon(hdc, polygonPoints, (*closedAreas)[i].size() - 1);
		delete[] polygonPoints;// free(polygonPoints);
	}
	//
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
	DeleteObject((HGDIOBJ)(HPEN)(playersPen));
	//ReleaseDC(hWnd, hdc);
}

VOID Drawing::DrawDots(UINT playersAmount)
{
	//HDC hdc = GetDC(hWnd);
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	for (UINT player = FIRST_PLAYER; player < FIRST_PLAYER + playersAmount; player++)
	{
		SetPlayerColors(hdc, player);
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
	DeleteObject((HGDIOBJ)(HPEN)(playersPen));
	//ReleaseDC(hWnd, hdc);
}

VOID Drawing::FindPhysicalCoordinates(INT moveNum)
{
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
		POINT dot = *(logicalToPhysical)[0][0];
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

VOID Drawing::HighliteDot(INT player, POINT dot)
{
	//HDC hdc = GetDC(hWnd);
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(DC_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetPlayerColors(hdc, player);
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
	DeleteObject((HGDIOBJ)(HPEN)(playersPen));
	//ReleaseDC(hWnd, hdc);
}

VOID Drawing::InitializeDotsMatrix()
{
	//std::vector<std::vector<PPOINT>> physicalCoordinatesMatrix(Drawing::fieldHeight);
	for (int i = 0; i < MAX_FIELD_HEIGHT; i++)
	{
		//std::vector<PPOINT> physicalCoordinatesArr(Drawing::fieldWidth);
		for (int j = 0; j < MAX_FIELD_WIDTH; j++)
		{
			dots[i][j]->state = EMPTY_POS;
			dots[i][j]->logicalCoordinate.x = i;
			dots[i][j]->logicalCoordinate.y = j;
			//physicalCoordinatesArr[j] = (PPOINT)malloc(sizeof(POINT));
		}
		//physicalCoordinatesMatrix[i] = physicalCoordinatesArr;
	}
	//logicalToPhysical = physicalCoordinatesMatrix;
}

VOID Drawing::InitializeLogFont()
{
	logFont.lfHeight = cellSize;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_DEMIBOLD;
	logFont.lfItalic = TRUE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = FALSE;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_DECORATIVE;
	wcscpy_s(logFont.lfFaceName, SCORE_FONTNAME);
}

BOOLEAN Drawing::IsOnField(INT x, INT y)
{
	INT marginLR = (windowWidth - fieldWidth*cellSize) / 2,
		marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	return (x > marginLR && x<(marginLR + fieldWidth*cellSize) && y>marginTB && y < (marginTB + fieldHeight*cellSize));
}

VOID Drawing::LineField()
{
	INT marginLR = (windowWidth - fieldWidth*cellSize) / 2, marginTB = (windowHeight - fieldHeight*cellSize) / 2;
	LineRect(marginLR, marginTB, marginLR + fieldWidth*cellSize, marginTB + fieldHeight*cellSize);
}

VOID Drawing::LineRect(INT left, INT top, INT right, INT bottom)
{
	//HDC hdc = GetDC(hWnd);
	HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(NULL_PEN));
	HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
	//SelectObject(hdc, );
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
	Rectangle(hdc, left, top, right, bottom);
	SelectObject(hdc, GetStockObject(DC_PEN));
	//int countHorisontal = 0, countVertical = 0;
	int x = left;
	while (x <= right)
	{
		MoveToEx(hdc, x, top, NULL);
		LineTo(hdc, x, bottom);
		x += cellSize;
		//countHorisontal++;
	}

	int y = top;
	while (y <= bottom)
	{
		MoveToEx(hdc, left, y, NULL);
		LineTo(hdc, right, y);
		y += cellSize;
		//countVertical++;
	}
	//HPEN shadowPen = CreatePen(PS_SOLID, 7, SHADOW_COLOR);
	////HBRUSH shadowBrush = CreateSolidBrush(SHADOW_COLOR);
	////SelectObject(hdc, shadowBrush);
	//SelectObject(hdc, shadowPen);
	//MoveToEx(hdc, marginLR+4, marginTB + fieldHeight*cellSize + 4, NULL);
	//LineTo(hdc, marginLR + fieldWidth*cellSize + 4, marginTB + fieldHeight*cellSize + 4);
	//LineTo(hdc, marginLR + fieldWidth*cellSize + 4, marginTB+1);
	//DeleteObject((HGDIOBJ)(HPEN)(shadowPen));
	if (originalPen != NULL)
		SelectObject(hdc, originalPen);
	if (originalBrush != NULL)
		SelectObject(hdc, originalBrush);
	DeleteObject((HGDIOBJ)(HPEN)(playersPen));
	//ReleaseDC(hWnd, hdc);
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


VOID Drawing::SetPlayerColors(HDC hdc, UINT player)
{
	//HDC hdc = GetDC(hWnd);
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
			playersPen = CreatePen(PS_SOLID, penWidth, NOTEBOOK_FIRST_DOT_PEN);
			SelectObject(hdc, playersPen);
			SetDCBrushColor(hdc, NOTEBOOK_FIRST_DOT_BRUSH);
			SetTextColor(hdc, NOTEBOOK_FIRST_DOT_BRUSH);
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
			playersPen = CreatePen(PS_SOLID, penWidth, NOTEBOOK_SECOND_DOT_PEN);
			SelectObject(hdc, playersPen);
			SetDCBrushColor(hdc, NOTEBOOK_SECOND_DOT_BRUSH);
			SetTextColor(hdc, NOTEBOOK_SECOND_DOT_BRUSH);
		}
		break;
		}
	}
	break;
	case THIRD_PLAYER:
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
			playersPen = CreatePen(PS_SOLID, penWidth, NOTEBOOK_THIRD_DOT_PEN);
			SelectObject(hdc, playersPen);
			SetDCBrushColor(hdc, NOTEBOOK_THIRD_DOT_BRUSH);
			SetTextColor(hdc, NOTEBOOK_THIRD_DOT_BRUSH);
		}
		break;
		}
	}
	break;
	}
	//ReleaseDC(hWnd, hdc);
}

VOID Drawing::ShowBackground()
{
	//HDC hdc = GetDC(hWnd);
	HDC memDC = CreateCompatibleDC(hdc);
	originalBmp = (HBITMAP)SelectObject(memDC, hBmpBackground);
	BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDC, 0, 0, SRCCOPY);
	if (originalBmp!=NULL)
		SelectObject(memDC, originalBmp);
	DeleteDC(memDC);
	//ReleaseDC(hWnd, hdc);
	//BitBlt(hdc, 0, 0, x, y, memDC, 0, 0, SRCCOPY);
}

VOID Drawing::ShowRecords(vector<PWCHAR> recordsLines)
{
	INT marginLR = (windowWidth - RECORDS_FIELD_WIDTH) / 2, marginTB = (windowHeight - RECORDS_FIELD_HEIGHT) / 2;
	LineRect(marginLR, marginTB, marginLR + RECORDS_FIELD_WIDTH, marginTB + RECORDS_FIELD_HEIGHT);
	SetBkMode(hdc, TRANSPARENT);
	HFONT hFont = CreateFontIndirectW(&logFont);
	marginLR += (RECORDS_FIELD_WIDTH - SCORES_WIDTH) / 2;
	marginTB += SCORES_MARGIN / 2;
	for (INT record=0; record < recordsLines.size(); record++)
	{
		RECT textRect;
		SetRect(&textRect, marginLR, marginTB + record*SCORES_HEIGHT, marginLR + SCORES_WIDTH, marginTB + SCORES_HEIGHT*(record + 1));
		DrawTextW(hdc, recordsLines[record], -1, &textRect, DT_WORDBREAK | DT_VCENTER | DT_CENTER);
	}
	DeleteObject((HGDIOBJ)(HFONT)(hFont));
}

VOID Drawing::ShowScores(vector<PWCHAR> playersNames, vector<INT> *scores)
{
	LineRect(SCORES_MARGIN/2, SCORES_MARGIN/2, SCORES_MARGIN*3/2 + SCORES_WIDTH, SCORES_MARGIN*3/2 + SCORES_HEIGHT*scores->size()-10);
	SetBkMode(hdc,TRANSPARENT);
	HFONT hFont = CreateFontIndirectW(&logFont);
	HGDIOBJ originalFont = SelectObject(hdc, hFont);
	for (INT player = FIRST_PLAYER; player < FIRST_PLAYER + scores->size(); player++)
	{
		HGDIOBJ originalPen = SelectObject(hdc, GetStockObject(NULL_PEN));
		HGDIOBJ originalBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
		RECT textRect;
		SetRect(&textRect, SCORES_MARGIN, SCORES_MARGIN+player*SCORES_HEIGHT, SCORES_MARGIN + SCORES_WIDTH, SCORES_MARGIN + SCORES_HEIGHT*(player+1));
		SetPlayerColors(hdc, player);
		PWCHAR text = new WCHAR[MAX_NICKNAME + 6];
		/*PWCHAR strPlayerNum = new WCHAR[2];
		_itow_s(player+1, strPlayerNum, 2, 10);
		wcscpy_s(text, MAX_NICKNAME+6, L"Player");
		wcscat_s(text, MAX_NICKNAME+6, strPlayerNum);*/
		wcscpy_s(text, MAX_NICKNAME + 6, playersNames[player]);
		wcscat_s(text, MAX_NICKNAME+6, L": ");
		PWCHAR strPlayerScore = new WCHAR[4];
		_itow_s((*scores)[player], strPlayerScore, 4, 10);
		wcscat_s(text, MAX_NICKNAME+6, strPlayerScore);
		DrawTextW(hdc, text, -1, &textRect, DT_WORDBREAK | DT_VCENTER | DT_CENTER);
		//delete[] strPlayerNum;
		delete[] text;
		delete[] strPlayerScore;
		if (originalPen != NULL)
			SelectObject(hdc, originalPen);
		if (originalBrush != NULL)
			SelectObject(hdc, originalBrush);
		DeleteObject((HGDIOBJ)(HPEN)(playersPen));
		//logFont.lfWeight = FW_MEDIUM;
	}
	if (originalFont != NULL)
		SelectObject(hdc, originalFont);
	DeleteObject((HGDIOBJ)(HFONT)(hFont));
}

Drawing::~Drawing()
{
}
