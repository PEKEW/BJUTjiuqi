#include "pch.h"
#include "header.h"

double historyTable[BOARD_SIZE][BOARD_SIZE] = { 0 };

int addHistory(JiuMove m,int depWeight)
{
	for (auto pos : m.capturePositions) {
		historyTable[pos[0]][pos[1]] += depWeight;
	}
	return 0;
}

int declineHistory()
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			historyTable[i][j] = sqrt(historyTable[i][j]);
		}
	}
	return 0;
}



void JiuMove::initPos(int xStart, int yStart, int xEnd, int yEnd)
{
	xStartPosition = xStart;
	yStartPosition = yStart;
	xEndPosition = xEnd;
	yEndPosition = yEnd;
}

JiuMove::JiuMove(int xStart, int yStart, int xEnd, int yEnd):
	xStartPosition(xStart),
	yStartPosition(yStart),
	xEndPosition(xEnd),
	yEndPosition(yEnd)
{
}

int JiuMove::addJumpPos(int x,int y)
{
	jumpPositions.push_back({ x,y });
	jumpCaptureCount = jumpPositions.size();
	return 0;
}

int JiuMove::addCapPos(int x, int y)
{
	capturePositions.push_back({ x,y });
	captureCount = capturePositions.size();
	return 0;
}


