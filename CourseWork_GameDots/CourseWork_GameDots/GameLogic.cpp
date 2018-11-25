#include "stdafx.h"
#include "GameLogic.h"
#include "Drawing.h"
#include <vector>
#include <algorithm>

using namespace std;

INT GameLogic::moveNum = 0;
INT GameLogic::playersAmount = 3;

GameLogic::GameLogic()
{
	capturedDotsAmounts = new vector<INT>(playersAmount);
	for (INT i = 0; i < playersAmount; i++)
	{
		(*capturedDotsAmounts)[i] = 0;
	}
}

VOID GameLogic::AddVertex(int num, int xLogical, int yLogical, vector<vector<PPOINT>> *physicalCoordinates)
{
	VERTEX newVertex;
	newVertex.num = num;
	newVertex.logicalCoordinate.x = xLogical;
	newVertex.logicalCoordinate.y = yLogical;
	vertexes.push_back(newVertex);
	if (num >= playersAmount*3)
	{
		FindClosedArea(physicalCoordinates);
	}
	moveNum++;
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
					if (binary_search(differentVector.begin(), differentVector.end(),cyclesTempSorted[i][num]))
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
	PVERTEX lastPlacedVertexPtr = &(vertexes[moveNum]);
	//std::vector<std::wstring> cycles;
	vector<vector<UINT>> cyclesFound;
	vector<UINT> firstCycle;// = new vector<UINT>(MAX_CYCLE_LENGTH);
	FindCycles(lastPlacedVertexPtr, lastPlacedVertexPtr, &firstCycle, &cyclesFound, 0);
	//delete(firstCycle);
	UINT cycleNum = 0;
	while (cycleNum < cyclesFound.size())
	{
		if (cyclesFound[cycleNum].size()<5)
		{
			cyclesFound[cycleNum].clear();
			cyclesFound.erase(cyclesFound.begin() + cycleNum);
		}
		else
		{
			cycleNum++;
		}
	}
	//for (int i = player; i <= moveNum; i += playersAmount)
	//	vertexes[i].isAvailable = TRUE;
	if (cyclesFound.size() > 0)
	{
		cyclesFound = ExcludeIncludedInOther(&cyclesFound);
		if (cyclesFound.size() > 0)
		{
			cycleNum = 0;
			while (cycleNum < cyclesFound.size())
			{
				INT capturedDots = FindOtherPlayersDots(&cyclesFound[cycleNum], physicalCoordinates);
				if (capturedDots==0)
				{
					cyclesFound.erase(cyclesFound.begin() + cycleNum);
				}
				else
				{
					closedAreas.push_back(cyclesFound[cycleNum]);
					(*capturedDotsAmounts)[player] += capturedDots;
					cycleNum++;
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
	//vector<UINT>* newCycle = new vector<UINT>(*cycle);
	//newCycle->push_back(current->num);
	cycle->push_back(current->num);
	//(*cycle)[recursionDepth] = current->num;
	current->isAvailable = FALSE;
	if (lastPlaced->num == current->num && recursionDepth > 2)
	{
		cyclesVector->push_back(*cycle);
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
				FindCycles(lastPlaced, &vertexes[i], cycle, cyclesVector, recursionDepth + 1);
			}
		}
		//free(newCycle);
		//newCycle = NULL;
		//cycle->pop_back();
	}
	else
	{
		/*free(newCycle);
		newCycle = NULL;*/
		//cycle->pop_back();
	}
	//(*cycle)[recursionDepth] = 0;
	cycle->pop_back();
	current->isAvailable = TRUE;
}

INT GameLogic::FindOtherPlayersDots(vector<UINT> *cycle, std::vector<vector<PPOINT>> *physicalCoordinates)
{
	INT count = 0;
	INT player = vertexes[(*cycle)[0]].num%playersAmount;
	INT theMostLeft = MAXINT, theMostRight = 0;
	INT theHighest = MAXINT, theLowest = 0;
	POINT *polygonPoints;
	polygonPoints = new POINT[cycle->size() - 1];//(POINT*)malloc(sizeof(POINT)*(cycle->size() - 1));
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
			if (CombineRgn(pointRgn, polygonRgn, pointRgn, RGN_AND) != NULLREGION)
			{
				vertexes[i].isAvailable = FALSE;
				count++;
			}
			DeleteObject((HGDIOBJ)(HRGN)(pointRgn));
		}
	}
	DeleteObject((HGDIOBJ)(HRGN)(polygonRgn));
	delete[] polygonPoints;
	//free(polygonPoints);
	return count;
}

VOID GameLogic::StartNewGame()
{
	vertexes.clear();
	closedAreas.clear();
	//capturedDotsAmounts->clear();
	for (INT i = 0; i < playersAmount; i++)
	{
		(*capturedDotsAmounts)[i] = 0;
	}
	moveNum = 0;
}

GameLogic::~GameLogic()
{
	delete capturedDotsAmounts;
}
