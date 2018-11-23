// Lab2.cpp: ���������� ����� ����� ��� ����������.
//

#include "stdafx.h"
#include "Lab2.h"
#include <string>
#include <sstream>
#include <iostream>
#include <locale>
#include <fstream>
#include "windows.h"
#include <vector>

#define MAX_LOADSTRING 100
#define IDM_OPEN 1

#define MAX_TABLE_ROWS_NUM 15
#define MAX_TABLE_COLUMNS_NUM 10
#define MAX_CELLTEXT_LENGTH 15

#define FONT_NAME L"Courier New"

// ���������� ����������:
HINSTANCE hInst;                                // ������� ���������
WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����

												// ��������� ���������� �������, ���������� � ���� ������ ����:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

char* tableText[MAX_TABLE_ROWS_NUM][MAX_TABLE_COLUMNS_NUM];
int tableRowsNum = 0, tableColumnsNum = 0;
int windowWidth, windowHeight, cellWidth, cellHeight;
int offsetsLR = 0, offsetsTB = 0;
bool evenlyHeight = true, evenlyWidth = true;
int* columnsWidths;
int* rowsHeights;

HDC hdc;
PAINTSTRUCT ps;
TEXTMETRIC tm;
HFONT hFont;
HBRUSH backgroundBrush, tableBrush;
//RECT cellRt;

COLORREF backgroundColor = COLOR_BACKGROUND;
//COLORREF tableColor = COLOR_TABLE;
//COLORREF tableNumberColor = COLOR_TABLENUMBER;
//COLORREF tableAccentNumberColor = COLOR_TABLEACCENT;
HMENU hMenu;
LOGFONT lf;



VOID InitializeFont()
{
	lf.lfHeight = 20; // ������ ���������� ������ ������ � ���������� ��������
	lf.lfWidth = 0; // ������ ���������� ������ ������ � ���������� ��������
	lf.lfEscapement = 0; // ���� ����� �������� ������� � ���� X ���������� � ������� ����� ��������
	lf.lfOrientation = 0; // ���� ����� �������� ������ ������� ������� � ���� X ���������� � ������� ����� ��������
	lf.lfWeight = FW_NORMAL; // ������� ������ � ��������� 0..1000
	lf.lfItalic = 0; // ��������� �����
	lf.lfUnderline = 0; // ������������ �����
	lf.lfStrikeOut = 0; // ����������� �����
	lf.lfCharSet = ANSI_CHARSET; // ����� ��������
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS; // �������� ������
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS; // �������� ���������
	lf.lfQuality = PROOF_QUALITY; // �������� ������
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN; // ������ �������� � ��������� �������
	wcscpy_s(lf.lfFaceName, FONT_NAME); // �������� ��������� ������
	hFont = CreateFontIndirect(&lf);
}

char* ChooseFile(HWND hWnd)
{
	char* fName = (char *)malloc(sizeof(char) * MAX_LOADSTRING);
	strcpy(fName, "C:\\Users\\ZlatKa\\Documents\\5 ���\\�����\\Lab2\\�������.txt");
	return fName;
}

BOOL ReadTableTextfromFile(char* path)
{
	for (int i = 0; i< MAX_TABLE_COLUMNS_NUM; i++)
		for (int j = 0; j< MAX_TABLE_COLUMNS_NUM; j++)
			tableText[i][j] = (char *)malloc(sizeof(char)*MAX_CELLTEXT_LENGTH);

	std::ifstream fin(path);
	if (!fin.is_open())
		return false;
	int rNum = 0;
	char lineBuff[MAX_TABLE_COLUMNS_NUM * MAX_CELLTEXT_LENGTH];
	char* cellBuff;
	while ((rNum < MAX_TABLE_ROWS_NUM) && !(fin.eof()))
	{
		fin.getline(lineBuff, MAX_TABLE_COLUMNS_NUM * MAX_CELLTEXT_LENGTH);
		int cNum = 0;
		cellBuff = strtok(lineBuff, ";\n");
		while ((cNum < MAX_TABLE_COLUMNS_NUM) && (cellBuff))
		{
			strcpy(tableText[rNum][cNum], cellBuff);
			cNum++;
			cellBuff = strtok(NULL, ";\n");
		}
		if (cNum > tableColumnsNum)
			tableColumnsNum = cNum;

		while (cNum < MAX_TABLE_COLUMNS_NUM)
		{
			free(tableText[rNum][cNum]);
			tableText[rNum][cNum] = (char *)malloc(sizeof(char));
			strcpy(tableText[rNum][cNum], "");
			cNum++;
		}

		rNum++;
	}
	fin.close();
	tableRowsNum = rNum;
	return true;
}

int FindTextHeight(char* text)
{
	SIZE size;
	GetTextExtentPoint32A(hdc, text, strlen(text), &size);
	return size.cy;
	//return 40;
	//return tm.tmHeight;
}

int FindTextWidth(char* text)
{
	SIZE size;
	GetTextExtentPoint32A(hdc, text, strlen(text), &size);
	return size.cx;
	//return 100;
	//return tm.tmAveCharWidth*strlen(text);
}

bool FindTableParamms(RECT &newWinRec)
{
	bool resized = true;
	if (newWinRec.bottom > 0)
	{
		windowHeight = newWinRec.bottom - newWinRec.top;
		windowWidth = newWinRec.right - newWinRec.left;
	}
	int rowsMaxHeights[MAX_TABLE_ROWS_NUM], columnsMaxWidths[MAX_TABLE_COLUMNS_NUM];
	for (int j = 0; j < tableColumnsNum; j++)
	{
		columnsMaxWidths[j] = FindTextWidth(tableText[0][j]);
	}
	int maxHeight = FindTextHeight(tableText[0][0]), maxWidth = columnsMaxWidths[0];
	for (int i = 0; i < tableRowsNum; i++)
	{
		rowsMaxHeights[i] = FindTextHeight(tableText[i][0]);
		for (int j = 0; j < tableColumnsNum; j++)
		{
			int h = FindTextHeight(tableText[i][j]), w = FindTextWidth(tableText[i][j]);
			if (h > rowsMaxHeights[i])
			{
				rowsMaxHeights[i] = h;
				if (h > maxHeight)
					maxHeight = h;
			}
			if (w > columnsMaxWidths[j])
			{
				columnsMaxWidths[j] = w;
				if (w > maxWidth)
					maxWidth = w;
			}
		}
	}

	int averageWidth = (windowWidth - 10) / tableColumnsNum;
	int averageHeight = windowHeight / tableRowsNum;

	if (averageWidth < maxWidth)
	{
		int sumMaxWidth = 0;
		for (int i = 0; i < tableColumnsNum; i++)
			sumMaxWidth += columnsMaxWidths[i];
		if (sumMaxWidth > windowWidth)
		{
			/*evenlyWidth = true;
			cellWidth = averageWidth;*/
			bool newWidthsFound = true;
			std::vector<int> newRowHeights(tableRowsNum);
			for (int i = 0; i < tableRowsNum; i++)
			{
				std::vector<int> newCellHeights(tableColumnsNum);
				std::vector<int> newCellWidths(tableColumnsNum);
				for (int j = 0; j < tableColumnsNum; j++)
				{
					if (strcmp(tableText[i][j], ""))
					{
						RECT cell;
						SetRect(&cell, 0, 0, averageWidth, averageHeight);
						int h = DrawTextA(hdc, tableText[i][j], strlen(tableText[i][j]), &cell, DT_CALCRECT | DT_WORDBREAK);
						newCellHeights[j] = h;
						newCellWidths[j] = cell.right - cell.left;
						/*if ((cell.right-cell.left) > averageWidth)
						{
						newWidthsFound = false;
						}*/
					}
				}
				if (newWidthsFound)
				{
					int maxHeight = newCellHeights[0];
					for (int j = 0; j < tableColumnsNum; j++)
					{
						if (newCellHeights[j] > maxHeight)
							maxHeight = newCellHeights[j];
						columnsMaxWidths[j] = newCellWidths[j];
					}
					newRowHeights[i] = maxHeight;
				}
			}
			if (newWidthsFound)
			{
				for (int i = 0; i < tableRowsNum; i++)
				{
					if (rowsMaxHeights[i] < newRowHeights[i])
					{
						rowsMaxHeights[i] = newRowHeights[i];
						if (maxHeight < newRowHeights[i])
						{
							maxHeight = newRowHeights[i];
						}
					}
				}
				evenlyWidth = false;
				int newSumMaxWidth = 0;
				for (int i = 0; i < tableColumnsNum; i++)
					newSumMaxWidth += columnsMaxWidths[i];
				if (newSumMaxWidth > windowWidth)
				{
					newWinRec.right = newWinRec.left + newSumMaxWidth + 10;
					resized = false;
				}
				columnsWidths = (int *)malloc(sizeof(int)*tableColumnsNum);
				for (int i = 0; i < tableColumnsNum; i++)
					columnsWidths[i] = columnsMaxWidths[i] + (windowWidth - 10 - newSumMaxWidth) / tableColumnsNum;

			}
		}
		else
		{
			evenlyWidth = false;
			columnsWidths = (int *)malloc(sizeof(int)*tableColumnsNum);
			for (int i = 0; i < tableColumnsNum; i++)
				columnsWidths[i] = columnsMaxWidths[i] + (windowWidth - 10 - sumMaxWidth) / tableColumnsNum;
		}
	}
	else
	{
		evenlyWidth = true;
		cellWidth = averageWidth;
	}

	if (averageHeight < maxHeight)
	{
		int sumMaxHeight = 0;
		for (int i = 0; i < tableRowsNum; i++)
			sumMaxHeight += rowsMaxHeights[i];
		if ((sumMaxHeight + 60) > windowHeight)
		{
			newWinRec.bottom = newWinRec.top + sumMaxHeight + 60;
		}
		evenlyHeight = false;
		rowsHeights = (int*)malloc(sizeof(int)*tableRowsNum);
		for (int i = 0; i < tableRowsNum; i++)
			rowsHeights[i] = rowsMaxHeights[i];
	}
	else
	{
		evenlyHeight = true;
		cellHeight = maxHeight;
	}
	return resized;
}

void PrintTable()
{
	if (tableText[0][0] == NULL)
		return;
	int tableWidth = 0;
	if (evenlyWidth)
		tableWidth = cellWidth*tableColumnsNum;
	else
	{
		for (int i = 0; i < tableColumnsNum; i++)
			tableWidth += columnsWidths[i];
	}
	int tableHeight = 0;
	if (evenlyHeight)
		tableHeight = cellHeight*tableRowsNum;
	else
	{
		for (int i = 0; i < tableRowsNum; i++)
			tableHeight += rowsHeights[i];
	}

	offsetsLR = (windowWidth - tableWidth) / 2;
	if (windowHeight < tableHeight)
	{
		windowHeight = tableHeight;
		offsetsTB = 0;
	}
	else
	{
		offsetsTB = (windowHeight - tableHeight) / 2;
	}

	int rowTop = offsetsTB, upperRowHeight = 0, leftColWidth = 0, height = 0, width = 0;
	for (int i = 0; i < tableRowsNum; i++)
	{
		rowTop += upperRowHeight;
		if (evenlyHeight)
			height = cellHeight;
		else
			height = rowsHeights[i];
		int colLeft = offsetsLR;
		for (int j = 0; j < tableColumnsNum; j++)
		{
			colLeft += leftColWidth;
			if (evenlyWidth)
				width = cellWidth;
			else
				width = columnsWidths[j];

			RECT cell;
			SetRect(&cell, colLeft, rowTop, colLeft + width, rowTop + height);
			DrawTextA(hdc, tableText[i][j], -1, &cell, DT_WORDBREAK | DT_VCENTER | DT_CENTER);
			FrameRect(hdc, &cell, tableBrush);
			leftColWidth = width;
		}
		leftColWidth = 0;
		upperRowHeight = height;
	}
}

void AddMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, IDM_OPEN, _T("������� ����"));
	SetMenu(hWnd, hMenu);
}

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
	LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ��������� ������������� ����������:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
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
	switch (message)
	{
	case WM_CREATE:
	{
		AddMenu(hWnd);
		InitializeFont();
		tableBrush = CreateSolidBrush(RGB(191, 186, 190));
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// ��������� ����� � ����:
		switch (wmId)
		{
		case IDM_OPEN:
		{
			char* filename = ChooseFile(hWnd);
			ReadTableTextfromFile(filename);
			RECT rc;
			rc.bottom = 0;
			hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hFont);
			FindTableParamms(rc);
			EndPaint(hWnd, &ps);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		SelectObject(hdc, hFont);
		GetTextMetricsW(hdc, &tm);
		PrintTable();
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		windowHeight = HIWORD(lParam);
		windowWidth = LOWORD(lParam);
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}
	break;
	case WM_SIZING:
	{
		hdc = BeginPaint(hWnd, &ps);
		SelectObject(hdc, hFont);
		RECT* newWindowRect = (RECT *)(lParam);;
		FindTableParamms(*newWindowRect);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		DeleteObject(backgroundBrush);
		DeleteObject(tableBrush);
		DeleteObject(hFont);
		PostQuitMessage(0);
	}
	break;
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
