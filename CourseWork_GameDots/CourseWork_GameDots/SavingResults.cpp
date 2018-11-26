#include "stdafx.h"
#include "resource.h"
#include "SavingResults.h"
#include "Dlgs.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

PWCHAR SavingResults::wsSaveGameDirectory;
PWCHAR SavingResults::wsSaveGameName;
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
				/*PWCHAR recordName=new WCHAR[MAX_NICKNAME];
				LoadStringW(hInst, recordNamesIDs[i], recordName, MAX_NICKNAME);
				WCHAR strRecordResult[4];
				LoadStringW(hInst, recordsIDs[i], strRecordResult, 4);
				INT recordResult = _wtoi(strRecordResult);
				*/
				PWCHAR recordLine=new WCHAR[MAX_NICKNAME+4];
				PWCHAR recordName;// = new WCHAR[MAX_NICKNAME];
				PWCHAR strRecordResult;// = new WCHAR[4];
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
				//delete strRecordResult;
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

VOID SavingResults::GetSaveGameDirectoryPath(PWCHAR programPath)
{
	/*wsSaveGameDirectory[MAX_PATH];
	wcscpy_s(wsSaveGameDirectory, MAX_PATH, programPath);
	PWCHAR pwcLastSlash = wcsrchr(wsSaveGameDirectory, L'\\');
	*(pwcLastSlash + sizeof(WCHAR)) = L'\0';
	wcscpy_s(pwcLastSlash + sizeof(WCHAR), SAVED_GAMES_DIRECTORY_LENGTH, SAVED_GAMES_DIRECTORY);*/
	wsSaveGameName[GAME_NAME_LENGTH];

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

VOID SavingResults::SaveRecords()
{
	//HANDLE hResource = BeginUpdateResourceW(name, FALSE);
	//BOOL result;
	////HMODULE hExe = LoadLibraryW(name);
	//if (hResource != NULL)
	//{
	//	for (INT i = 0; i < RECORDS_AMOUNT; i++)
	//	{
	//		PWCHAR recordName = recordsTable[i]->name;
	//		INT recordResult = recordsTable[i]->result;
	//		WCHAR strRecordResult[4];
	//		_itow_s(recordResult, strRecordResult, 4, 10);
	//		//HRSRC hResName = FindResourceW(hExe, MAKEINTRESOURCE(recordNamesIDs[i]), RT_STRING);
	//		result = UpdateResourceW(hResource, RT_STRING, MAKEINTRESOURCEW(recordNamesIDs[i]), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), recordName,MAX_NICKNAME);
	//		//HRSRC hResResult = FindResourceW(hExe, MAKEINTRESOURCE(recordsIDs[i]), RT_STRING);
	//		result = UpdateResourceW(hResource, RT_STRING, MAKEINTRESOURCEW(recordsIDs[i]), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), strRecordResult, 4);
	//	}
	//	result = EndUpdateResourceW(hResource, FALSE);
	//}
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
