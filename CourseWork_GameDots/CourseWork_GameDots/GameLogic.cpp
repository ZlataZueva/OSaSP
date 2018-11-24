#include "stdafx.h"
#include "GameLogic.h"
#include "Drawing.h"
#include <vector>
#include <algorithm>

using namespace std;

INT GameLogic::moveNum = 0;
INT GameLogic::playersAmount = 2;

VOID GameLogic::AddVertex(int num, int xLogical, int yLogical, vector<vector<PPOINT>> *physicalCoordinates)
{
	VERTEX newVertex;
	newVertex.num = num;
	newVertex.logicalCoordinate.x = xLogical;
	newVertex.logicalCoordinate.y = yLogical;
	vertexes.push_back(newVertex);
	if (num > 5)
	{
		FindClosedArea(physicalCoordinates);
	}
	//newVertex.isAvailable = TRUE;
	//for (int )
}

BOOLEAN CompareVectors(std::vector<UINT> a, std::vector<UINT> b)
{
	/*std::vector<UINT> *vA = ((std::vector<UINT> *)a);
	std::vector<UINT> *vB = ((std::vector<UINT> *)b);*/
	if (a.size() > b.size())
		return TRUE;
	return FALSE;
}

vector<vector<UINT>> ExcludeIncludedInOther(vector<vector<UINT>> *cyclesFound)
{
	sort(cyclesFound->begin(), cyclesFound->end(), CompareVectors);
	vector<vector<UINT>> cyclesTempSorted = *cyclesFound;
	for (UINT i = 0; i < cyclesTempSorted.size(); i++)
	{
		sort(cyclesTempSorted[i].begin(), cyclesTempSorted[i].end());
	}
	vector<UINT> differentCyclesNums;
	differentCyclesNums.push_back(0);
	for (UINT i = 1; i < cyclesTempSorted.size(); i++)
	{
		BOOLEAN isToBeIncluded = TRUE;
		for (UINT j = 0; j < differentCyclesNums.size(); j++)
		{
			BOOLEAN isDifferent = TRUE;
			BOOLEAN isPartOfAnother = TRUE;
			vector<UINT> differentVector = cyclesTempSorted[differentCyclesNums[j]];
			if (cyclesTempSorted[i] == differentVector)
			{
				isDifferent = FALSE;
			}
			else
			{
				for (UINT num = 0; num < cyclesTempSorted[i].size()-2; num++)
				{
					if (cyclesTempSorted[i][num] != differentVector[num])
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

BOOLEAN GameLogic::FindClosedArea(vector<vector<PPOINT>> *physicalCoordinates)
{
	BOOLEAN isFound = FALSE;
	int player = moveNum%playersAmount;
	PVERTEX lastPlacedVertexPtr = &(this->vertexes[moveNum]);
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
				if (!FindOtherPlayersDots(&cyclesFound[i], physicalCoordinates))
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

VOID GameLogic::FindCycles(PVERTEX lastPlaced, PVERTEX current, vector<UINT> *cycle, vector<vector<UINT>> *cyclesVector, UINT recursionDepth)
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
		for (int i = player; i <= moveNum; i += playersAmount)
		{
			if (i != current->num && abs(vertexes[i].logicalCoordinate.x - current->logicalCoordinate.x) <= 1 &&        //если соседняя
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

BOOLEAN GameLogic::FindOtherPlayersDots(vector<UINT> *cycle, std::vector<vector<PPOINT>> *physicalCoordinates)
{
	BOOLEAN isFound = FALSE;
	INT player = vertexes[(*cycle)[0]].num%playersAmount;
	INT theMostLeft = MAXINT, theMostRight = 0;
	INT theHighest = MAXINT, theLowest = 0;
	POINT *polygonPoints;
	polygonPoints = (POINT*)malloc(sizeof(POINT)*(cycle->size() - 1));
	for (UINT i = 0; i < cycle->size() - 1; i++)
	{
		POINT logicalCoordinate = vertexes[(*cycle)[i]].logicalCoordinate;
		if (logicalCoordinate.x > theMostRight)
			theMostRight = logicalCoordinate.x;
		if (logicalCoordinate.x < theMostLeft)
			theMostLeft = logicalCoordinate.x;
		if (logicalCoordinate.y < theHighest)
			theHighest = logicalCoordinate.y;
		if (logicalCoordinate.y > theLowest)
			theLowest = logicalCoordinate.y;

		POINT physicalCoordinate = *(*physicalCoordinates)[logicalCoordinate.x][logicalCoordinate.y];
		polygonPoints[i] = physicalCoordinate;
	}
	HRGN polygonRgn = CreatePolygonRgn(polygonPoints, cycle->size() - 1, WINDING);
	for (UINT i = 0; i < vertexes.size(); i++)
	{
		if (vertexes[i].num%playersAmount != player && //если не своя
			vertexes[i].isAvailable &&				   //если не захвачена
			vertexes[i].logicalCoordinate.y > theHighest  && //если ниже самой верхней
			vertexes[i].logicalCoordinate.y < theLowest &&
			vertexes[i].logicalCoordinate.x>theMostLeft &&    //если правее левой границы
			vertexes[i].logicalCoordinate.x<theMostRight)     //если левее правой границы
		{
			POINT physicalCoordinate = *(*physicalCoordinates)[vertexes[i].logicalCoordinate.x][vertexes[i].logicalCoordinate.y];
			HRGN pointRgn = CreateEllipticRgn(physicalCoordinate.x - 1, physicalCoordinate.y - 1, physicalCoordinate.x + 1, physicalCoordinate.y + 1);
			if (CombineRgn(polygonRgn, polygonRgn, pointRgn, RGN_AND) != NULLREGION)
			{
				vertexes[i].isAvailable = FALSE;
				isFound = TRUE;
			}
		}
	}
	return isFound;
}

GameLogic::GameLogic()
{
}


GameLogic::~GameLogic()
{
}
