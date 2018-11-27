#include "stdafx.h"
#include "resource.h"
#include "SavingResults.h"
#include "Dlgs.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include "Drawing.h"

using namespace std;

PWCHAR SavingResults::wsSaveGamePath;
vector<PWCHAR> SavingResults::playersNames;
vector<SavingResults::PRECORD> recordsTable;
//INT recordNamesIDs[RECORDS_AMOUNT]{IDS_RECORD1_NAME, IDS_RECORD2_NAME, IDS_RECORD3_NAME, IDS_RECORD4_NAME, IDS_RECORD5_NAME, IDS_RECORD6_NAME, IDS_RECORD7_NAME, IDS_RECORD8_NAME, IDS_RECORD9_NAME, IDS_RECORD10_NAME};
//INT recordsIDs[RECORDS_AMOUNT]{IDS_RECORD1, IDS_RECORD2, IDS_RECORD3, IDS_RECORD4, IDS_RECORD5, IDS_RECORD6, IDS_RECORD7, IDS_RECORD8, IDS_RECORD9, IDS_RECORD10};

SavingResults::SavingResults()
{
}

VOID SavingResults::ReadRecordsTable()
{
	if (recordsTable.size() == 0)
	{
		PWCHAR fileName = new WCHAR[RECORDS_FILENAME_LENGTH];
		wcscpy_s(fileName, RECORDS_FILENAME_LENGTH, RECORDS_FILE_NAME);
		wifstream fileInput(fileName);
		if (fileInput.is_open())
		{
			for (INT i = 0; i < RECORDS_AMOUNT; i++)
			{
				PWCHAR recordLine=new WCHAR[MAX_NICKNAME+4];
				PWCHAR recordName;
				PWCHAR strRecordResult;
				fileInput.getline(recordLine, MAX_NICKNAME + 4);
				PWCHAR* context = new PWCHAR;
				recordName = wcstok_s(recordLine, L": \n",context);
				strRecordResult = wcstok_s(NULL, L": \n", context);
				delete context;
				INT recordResult = _wtoi(strRecordResult);
				PRECORD record = new RECORD();
				record->name = new WCHAR[MAX_NICKNAME];
				wcscpy_s(record->name, MAX_NICKNAME, recordName);
				record->result = recordResult;
				recordsTable.push_back(record);
				delete recordLine;
			}
			fileInput.close();
		}
		else
		{
			for (INT i = 0; i < RECORDS_AMOUNT; i++)
			{
				PRECORD record = new RECORD();
				record->name = new WCHAR[MAX_NICKNAME];
				wcscpy_s(record->name, MAX_NICKNAME, L"-");
				record->result = 0;
				recordsTable.push_back(record);
			}
		}
		delete fileName;
	}
}

VOID SavingResults::GetSaveGamePath()
{
	CreateDirectoryW(SAVED_GAMES_DIRECTORY, NULL);
	SYSTEMTIME time;
	GetLocalTime(&time);
	PWCHAR strDay = new WCHAR[3];
	PWCHAR strMonth = new WCHAR[3];
	PWCHAR strYear = new WCHAR[5];
	PWCHAR strHour = new WCHAR[3];
	PWCHAR strMinutes = new WCHAR[3];
	_itow_s(time.wDay, strDay, 3, 10);
	_itow_s(time.wMonth,strMonth, 3, 10);
	_itow_s(time.wYear, strYear, 5, 10);
	_itow_s(time.wHour, strHour, 3, 10);
	_itow_s(time.wMinute, strMinutes, 3, 10);
	wsSaveGamePath = new WCHAR[GAME_NAME_LENGTH+ SAVED_GAMES_DIRECTORY_LENGTH];
	wcscpy_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, SAVED_GAMES_DIRECTORY);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L"\\");
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, strDay);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L".");
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, strMonth);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L".");
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, strYear);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L";");
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, strHour);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L":");
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, strMinutes);
	wcscat_s(wsSaveGamePath, GAME_NAME_LENGTH + SAVED_GAMES_DIRECTORY_LENGTH, L".dot");
	delete strDay;
	delete strMonth;
	delete strYear;
	delete strMinutes;
	delete strHour;
}

vector<PWCHAR> SavingResults::GetStringsRecords()
{
	ReadRecordsTable();
	vector<PWCHAR> stringsRecords;
	for (INT i = 0; i < RECORDS_AMOUNT; i++)
	{
		PWCHAR recordLine = new WCHAR[MAX_NICKNAME + 5];
		PWCHAR strRecordResult = new WCHAR[4];
		_itow_s(recordsTable[i]->result, strRecordResult, 4, 10);
		wcscpy_s(recordLine, MAX_NICKNAME + 5, recordsTable[i]->name);
		wcscat_s(recordLine, MAX_NICKNAME + 5, L": ");
		wcscat_s(recordLine, MAX_NICKNAME + 5, strRecordResult);
		wcscat_s(recordLine, MAX_NICKNAME + 5, L"\n");
		delete strRecordResult;
		stringsRecords.push_back(recordLine);
	}
	return stringsRecords;
}

VOID SavingResults::SaveGameInFile(BYTE playersAmount, vector<vector<WORD>> closedAreas, vector<GameLogic::VERTEX> vertexes)
{
	GetSaveGamePath();
	HANDLE hFile = CreateFileW(wsSaveGamePath,GENERIC_READ| GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != NULL)
	{
		//1.количество игроков в BYTE 2.замкнутые области как массивы WORD 3. двумерный массив размера MAX_FIELD_HEIGHT x MAX_FIELD_WIDTH с номерами вершин в WORD
		WORD** areas = new WORD*[closedAreas.size()];
		for (INT i = 0; i < closedAreas.size(); i++)
		{
			areas[i] = new WORD[closedAreas[i].size()];
			for (INT j = 0; j < closedAreas[i].size(); j++)
			{
				areas[i][j] = closedAreas[i][j];
			}
		}
		WORD** matrix = new WORD*[MAX_FIELD_HEIGHT]; 
		for (INT i = 0; i < MAX_FIELD_HEIGHT; i++)
		{
			matrix[i] = new WORD[MAX_FIELD_WIDTH];
			for (INT j = 0; j < MAX_FIELD_WIDTH; j++)
			{
				matrix[i][j] = (WORD)(-1);
			}
		}
		for (INT i = 0; i < vertexes.size(); i++)
		{
			matrix[vertexes[i].logicalCoordinate.x][vertexes[i].logicalCoordinate.y] = i;
		}
		/*DWORD bytesWritten;
		WriteFile(hFile, &playersAmount, 1, &bytesWritten, NULL);
		for (INT i = 0; i < MAX_FIELD_HEIGHT; i++)
		{
			for (INT j = 0; j < MAX_FIELD_WIDTH; j++)
			{
				WriteFile(hFile, &(matrix[i][j]), 2, &bytesWritten, NULL);
			}
		}
		for (INT i = 0; i < closedAreas.size(); i++)
		{
			for (INT j = 0; j < closedAreas[i].size(); j++)
			{
				WriteFile(hFile, &(areas[i][j]), 2, &bytesWritten, NULL);
			}
		}*/
		DWORD maxFileSize = 1;
		HANDLE hFileMap = CreateFileMappingW(hFile, NULL, PAGE_WRITECOPY, HIWORD(maxFileSize), LOWORD(maxFileSize), SAVE_FILEMAP_NAME);
		if (hFileMap != NULL)
		{
			VOID* viewAddres = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
			if (viewAddres != NULL)
			{
				UINT countBytes = 0;
				BYTE* buffer =(BYTE*) viewAddres;
				*buffer = playersAmount;
				buffer++;
				countBytes++;
				for (INT i = 0; i < MAX_FIELD_HEIGHT; i++)
				{
					for (INT j = 0; j < MAX_FIELD_WIDTH; j++)
					{
						*(WORD*)buffer = matrix[i][j];
						buffer += 2;
						countBytes += 2;
					}
				}
				for (INT i = 0; i < closedAreas.size(); i++)
				{
					for (INT j = 0; j < closedAreas[i].size(); j++)
					{
						*(WORD*)buffer = areas[i][j];
						buffer += 2;
						countBytes += 2;
					}
				}
				FlushViewOfFile(viewAddres, countBytes);
				UnmapViewOfFile(viewAddres);
				for (INT i = 0; i < closedAreas.size(); i++)
				{
					delete[] areas[i];
				}
				delete[] areas;
				for (INT i = 0; i < MAX_FIELD_HEIGHT; i++)
				{
					delete[] matrix[i];
				}
				delete[] matrix;
			}
			CloseHandle(hFileMap);
		}
		else
		{
			DWORD error = GetLastError();
			//error++;
		}
		CloseHandle(hFile);
	}
	delete wsSaveGamePath;
}

VOID SavingResults::SaveRecords()
{
	vector<PWCHAR> recordLines = GetStringsRecords();
	PWCHAR fileName = new WCHAR[RECORDS_FILENAME_LENGTH];
	wcscpy_s(fileName, RECORDS_FILENAME_LENGTH, RECORDS_FILE_NAME);
	wofstream fileOutput;
	fileOutput.open(fileName);
	for (INT i = 0; i < RECORDS_AMOUNT; i++)
	{
		fileOutput << recordLines[i];
	}
	fileOutput.close();
}

BOOLEAN CompareRecords(SavingResults::PRECORD a, SavingResults::PRECORD b)
{
	if (a->result > b->result)
		return TRUE;
	else 
		return FALSE;
}

VOID SavingResults::UpdateRecords(vector<INT>* results)
{
	for (INT player = 0; player < results->size(); player++)
	{
		PRECORD record = new RECORD();
		record->name = playersNames[player];
		record->result = (*results)[player];
		recordsTable.push_back(record);
	}
	sort(recordsTable.begin(), recordsTable.end(), CompareRecords);
	for (INT i = 0; i < results->size(); i++)
	{
		recordsTable.pop_back();
	}
}

SavingResults::~SavingResults()
{
}
