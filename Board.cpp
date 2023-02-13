#include "pch.h"
#include "Board.h"
#include "Zobrist.h"
#include <array>
#include <windows.h>

int drawEval(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2]);

bool Board::weakNotJumpSingle = true;

Board::Board()
{
	//初始化辅助边界
	for (int i = 0; i < BOARD_SIZE + 2; i++) {
		states[0][i] = ILLEGAL_PLAYER;
		states[BOARD_SIZE + 1][i] = ILLEGAL_PLAYER;
		states[i][0] = ILLEGAL_PLAYER;
		states[i][BOARD_SIZE + 1] = ILLEGAL_PLAYER;
	}
}


Board::~Board()
{
}

const WORD lastMoveColor =  FOREGROUND_RED | FOREGROUND_GREEN;
const WORD lastCapColor = FOREGROUND_INTENSITY | FOREGROUND_RED;
int Board::drawWithMove(JiuMove move) {
	//保存系统设置
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	WORD saved_attributes;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	saved_attributes = consoleInfo.wAttributes;


	if (move.moveState == ERROR_PIECE)
		return draw();
	std::cout << "当前场上棋子有" << countPieces << "个，" 
		<< stateText[1] << "有" << countPlayerPieces[1] << "个，"
		<< stateText[2] << "有" << countPlayerPieces[2] << "个，"
		<< std::endl;
	std::cout << std::setw(DRAW_SIZE) << ' ';
	for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++)
	{
		std::cout << std::setw(DRAW_SIZE) << j - 1 + ROW_OUTPUT_ADJUST;
	}
	std::cout << std::endl;
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		std::cout << std::setw(DRAW_SIZE) << char('A' + i - 1);
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++)
		{
			bool isStart = (i == (move.xStartPosition + 1) && j == (move.yStartPosition + 1));
			bool isEnd= (i == (move.xEndPosition + 1) && j == (move.yEndPosition + 1));
			bool isCap = false;
			if (move.captureCount) {
				for (int k = 0; k < move.captureCount; k++) {
					isCap = isCap || (i == (move.capturePositions[k][0] + 1) && j == (move.capturePositions[k][1] + 1));
				}
			}
			if (isStart || isEnd) {
				SetConsoleTextAttribute(hConsole, lastMoveColor);
				std::cout
					//<< "\033[33m"
					<< std::setw(DRAW_SIZE)
					<< stateText[(int)states[i][j]];
					//<< "\033[0m";
				SetConsoleTextAttribute(hConsole, saved_attributes);
			}
			else if (isCap) {
				SetConsoleTextAttribute(hConsole, lastCapColor);
				std::cout
					//<< "\033[31m"
					<< std::setw(DRAW_SIZE)
					<< stateText[(int)states[i][j]];
					//<< "\033[0m";
				SetConsoleTextAttribute(hConsole, saved_attributes);
			}
			else
				std::cout << std::setw(DRAW_SIZE) << stateText[(int)states[i][j]];
		}
		std::cout << std::endl;
	}
	return 0;
}



int Board::resetPieceState(int x, int y)
{
	countPlayerPieces[states[x + 1][y + 1]]--;
	countPlayerPieces[NON_PLAYER]++;
	countPieces = countPlayerPieces[1] + countPlayerPieces[2];
	ZbHash = ZbHash ^ zobrist.hash[x*BOARD_SIZE + y][states[x + 1][y + 1] - 1];
	states[x + 1][y + 1] = NON_PLAYER;
	return 0;
}

int Board::addPieceState(int x, int y, ChessState state)
{
	countPlayerPieces[NON_PLAYER]--;
	countPlayerPieces[state]++;
	countPieces = countPlayerPieces[1] + countPlayerPieces[2];
	ZbHash = ZbHash ^ zobrist.hash[x*BOARD_SIZE + y][state-1];
	states[x + 1][y + 1] = state;
	return 0;
}


const int Board::doMoves(JiuMove move)
{
	int &xs = move.xStartPosition,
		&xe = move.xEndPosition,
		&ys = move.yStartPosition,
		&ye = move.yEndPosition;

	if (outSide(xs, ys)) {
		//std::cout << "JiuMove起点越界\n";
		return -1;
	}
	ChessState oppo;
	bool isEndding = false;
	switch (move.moveState)
	{
	case ADD_PIECE:
		if (isState(xs, ys, NON_PLAYER)) {
			addPieceState(xs, ys, move.player);
			if (countPieces == BOARD_PIECE_LIMIT)
				boardState = MOVE;
		}
		else {
			std::cout << "非法布局\n";
			return -1;
		}
		break;

	case JUMP_PIECE:
		if (boardState == FLY) {
			ChessState NonFlyPlayer = ILLEGAL_PLAYER;//非飞子方不可单跳
			if (countPlayerPieces[1] == countPlayerPieces[2])
				NonFlyPlayer = NON_PLAYER;
			else
				NonFlyPlayer = (countPlayerPieces[1] > countPlayerPieces[2]) ? FIRST_PLAYER : SECOND_PLAYER;
			if (move.jumpCaptureCount == 1 && (weakNotJumpSingle|| move.player == NonFlyPlayer)) {
				//std::cout<<"非飞子方不可单跳\n";
				return -1;
			}
		}

	case STEP_PIECE:
		if (!isState(xs, ys, move.player)) {
			//std::cout<<"挪动非己方子\n";
			return -1;
		}
		if (!isState(xe, ye, NON_PLAYER)) {
			//std::cout<<"没有移动到空地上\n";
			return -1;
		}
		addPieceState(xe, ye, move.player);
		resetPieceState(xs, ys);
		oppo = getOppoPlayer(move.player);
		for (auto item : move.capturePositions) {
			if (isState(item[0], item[1], oppo)) {
				resetPieceState(item[0], item[1]);
			}
			else {
				//std::cout << "JiuMove错误：试图提去空白\n";
				return -2;
			}
		}
		if (countPlayerPieces[1] < FLY_MODE_LIMIT) {
			if (countPlayerPieces[1] < 4) {
				isEndding = true;
			}
			else {
				boardState = FLY;
			}
		}
		if (countPlayerPieces[2] < FLY_MODE_LIMIT) {
			if (countPlayerPieces[2] < 4) {
				isEndding = true;
			}
			else {
				boardState = FLY;
			}
		}
		if (isEndding)
			boardState = ENDDING;
		break;
	case FLY_PIECE:
		if (!isState(xs, ys, move.player)) {
			//std::cout<<"挪动非己方子\n";
			return -1;
		}
		if (!isState(xe, ye, NON_PLAYER)) {
			//std::cout<<"没有移动到空地上\n";
			return -1;
		}
		addPieceState(xe, ye, move.player);
		resetPieceState(xs, ys);
		oppo = getOppoPlayer(move.player);
		for (auto item : move.capturePositions) {
			if (isState(item[0], item[1], oppo)) {
				resetPieceState(item[0], item[1]);
			}
			else {
				return -2;
			}
		}
		if (countPlayerPieces[1] < 4 || countPlayerPieces[2] < 4)
			boardState = ENDDING;
		break;
	}
	return 0;
}
int Board::removeCenterPieces()
{
	resetPieceState(BOARD_CENTER-1, BOARD_CENTER - 1);
	resetPieceState(BOARD_CENTER, BOARD_CENTER);
	return 0;
}
const int connerTrans[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};//坐上 右上 左下 右下

int Board::preMove(int xs, int ys, int xe, int ye)
{
	int cap = 0;
	ChessState saveS = getPieceState(xs, ys);
	ChessState saveE = getPieceState(xe, ye);
	setPieceState(xs, ys, NON_PLAYER);
	setPieceState(xe, ye, saveS);

	//提子规则部分

	/*
	规则1：成方吃
	算法：
	四个角落起始为true，上下左右如果不是本方子则更改为false
	顺序为	0↖ ↗1
			2↙ ↘3
	*/
	bool count[4] = { true,true,true,true };
	if (getPieceState(xe, ye-1) != saveS) {
		count[0] = false;
		count[2] = false;
	}
	if (getPieceState(xe, ye + 1) != saveS) {
		count[1] = false;
		count[3] = false;
	}
	if (getPieceState(xe-1, ye ) != saveS) {
		count[0] = false;
		count[1] = false;
	}
	if (getPieceState(xe+1, ye) != saveS) {
		count[2] = false;
		count[3] = false;
	}

	for (int i = 0; i < 4; i++)
		if (count[i] &&
			(saveS== getPieceState(xe+connerTrans[i][0],ye+ connerTrans[i][1]))
			)
			cap++;

	//提子规则结束

	setPieceState(xs, ys, saveS);
	setPieceState(xe, ye, saveE);
	return cap;
}




int convolution(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2], const int Kernel[81][4], ChessState player) {
	int index = 1;
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END - 1; i++) {
		index = states[i][1] * 3 + states[i + 1][1];
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END - 1; j++) {
			index = index % 9;
			index = index * 9 + states[i][j + 1] * 3 + states[i + 1][j + 1];

			evalFirst[i][j] += Kernel[index][0];
			evalFirst[i + 1][j] += Kernel[index][1];
			evalFirst[i][j + 1] += Kernel[index][2];
			evalFirst[i + 1][j + 1] += Kernel[index][3];
		}
	}
	return 0;
}



int countScore(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState player) {
	int countEF = 0;
	int countDir = 0;//计数用
	ChessState oppo = getOppoPlayer(player);
	int i = BOARD_FOR_START, j = BOARD_FOR_START;
	for (i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		for (j = BOARD_FOR_START; j < BOARD_FOR_END; j++) {
			if (evalFirst[i][j] != 0 && states[i][j] == NON_PLAYER) {
				if (evalFirst[i][j] > 192) {

					bool flag[4] = { false };
					for (int d = 0; d < 4; d++)
						if (states[i + trans[d][0]][j + trans[d][1]] == player)
							flag[d] = true;
					bool bflag[4] = { false };
					if (states[i + connerTrans[0][0]][j + connerTrans[0][1]] == player) {
						if (flag[0] == true && flag[1] == true) {
							bflag[2] = true;
							bflag[3] = true;
						}
					}
					if (states[i + connerTrans[1][0]][j + connerTrans[1][1]] == player) {
						if (flag[0] == true && flag[3] == true) {
							bflag[1] = true;
							bflag[2] = true;
						}
					}
					if (states[i + connerTrans[2][0]][j + connerTrans[2][1]] == player) {
						if (flag[2] == true && flag[1] == true) {
							bflag[0] = true;
							bflag[3] = true;
						}
					}
					if (states[i + connerTrans[3][0]][j + connerTrans[3][1]] == player) {
						if (flag[2] == true && flag[3] == true) {
							bflag[0] = true;
							bflag[1] = true;
						}
					}


					for (int d = 0; d < 4; d++) {
						if ((states[i + trans[d][0]][j + trans[d][1]] == player) && (evalFirst[i + trans[d][0]][j + trans[d][1]] > 64 && bflag[d])) {
							countEF += EVAL_DALIAN;
							if (evalFirst[i][j] >= (192*2))
								countEF += EVAL_DALIAN;
						}
						
					}
				}
				countDir = 0;
				for (int d = 0; d < 4; d++) {
					if (states[i + trans[d][0]][j + trans[d][1]] == player)
						countDir++;
				}
				if (countDir > 2)
					evalFirst[i][j] *= (countDir - 2);
				else
					evalFirst[i][j] = 0;
			}
			if (states[i][j] != oppo)
				countEF += evalFirst[i][j];
		}
	}
	return countEF;
}

int layoutCountScore(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState player) {
	int countEF = 0;
	int countDir = 0;//计数用
	ChessState oppo = getOppoPlayer(player);
	int i = BOARD_FOR_START, j = BOARD_FOR_START;
	for (i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		for (j = BOARD_FOR_START; j < BOARD_FOR_END; j++) {
			if (evalFirst[i][j] != 0 && (states[i][j] == NON_PLAYER|| states[i][j] == oppo)) {
				if (evalFirst[i][j] > 192) {
					bool flag[4] = { false };
					for (int d = 0; d < 4; d++)
						if (states[i + trans[d][0]][j + trans[d][1]] == player)
							flag[d] = true;
					bool bflag[4] = { false };
					if (states[i + connerTrans[0][0]][j + connerTrans[0][1]] == player) {
						if (flag[0] == true && flag[1] == true) {
							bflag[2] = true;
							bflag[3] = true;
						}
					}
					if (states[i + connerTrans[1][0]][j + connerTrans[1][1]] == player) {
						if (flag[0] == true && flag[3] == true) {
							bflag[1] = true;
							bflag[2] = true;
						}
					}
					if (states[i + connerTrans[2][0]][j + connerTrans[2][1]] == player) {
						if (flag[2] == true && flag[1] == true) {
							bflag[0] = true;
							bflag[3] = true;
						}
					}
					if (states[i + connerTrans[3][0]][j + connerTrans[3][1]] == player) {
						if (flag[2] == true && flag[3] == true) {
							bflag[0] = true;
							bflag[1] = true;
						}
					}


					for (int d = 0; d < 4; d++) {
						if ((states[i + trans[d][0]][j + trans[d][1]] == player) && (evalFirst[i + trans[d][0]][j + trans[d][1]] > 64 && bflag[d])) {
							countEF += EVAL_DALIAN;
							if (evalFirst[i][j] >= (192 * 2))
								countEF += EVAL_DALIAN;
						}

					}
				}
				countDir = 0;
				for (int d = 0; d < 4; d++) {
					if (states[i + trans[d][0]][j + trans[d][1]] == player)
						countDir++;
				}
				if (countDir > 2)
					evalFirst[i][j] *= (countDir - 2);
				//else
				//	evalFirst[i][j] = 0;
			}
			countEF += evalFirst[i][j];
			if(states[i][j]==oppo&&evalFirst[i][j]>=192)
				countEF += 10*evalFirst[i][j];
		}
	}
	return countEF;
}

int countFlyScore(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState player,int countPlayerPieces) {
	int countEF = 0;
	int countFlyPieces = countPlayerPieces - 3;
	ChessState oppo = getOppoPlayer(player);
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++) {	
			if (evalFirst[i][j] != 0&&states[i][j] == NON_PLAYER) {
				evalFirst[i][j] *= countFlyPieces;
			}
			if (states[i][j] != oppo)
				countEF += evalFirst[i][j];
		}
	}
	return countEF;
}


int Board::evalMethodFirst(bool flyEvalMode, bool debug) {
	int index = 1;
	int countEF = 0, countES = 0;

	int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	convolution(evalFirst, states, EvalKernelFirst, FIRST_PLAYER);
	if (flyEvalMode&&isFlyPlayer(FIRST_PLAYER))
		countEF = countFlyScore(evalFirst, states, FIRST_PLAYER, countPlayerPieces[FIRST_PLAYER]);
	else
		countEF = countScore(evalFirst, states, FIRST_PLAYER);
	if (debug) {
		draw();
		drawEval(evalFirst);
	}

	int evalSecond[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	convolution(evalSecond, states, EvalKernelSecond, FIRST_PLAYER);
	if (flyEvalMode&&isFlyPlayer(SECOND_PLAYER))
		countES = countFlyScore(evalSecond, states, SECOND_PLAYER, countPlayerPieces[SECOND_PLAYER]);
	else
		countES = countScore(evalSecond, states, SECOND_PLAYER);
	if (debug) {
		drawEval(evalSecond);
	}

	int value = countEF  - countES ;

	if (flyEvalMode) {
		int countKF = 0, countKS = 0;//击杀奖励，强迫推进至飞子阶段,KillFirst
		if (countPlayerPieces[1] < FLY_MODE_LIMIT) {
			countKS = (FLY_MODE_LIMIT - countPlayerPieces[1]) * KILL_WEIGHT;
		}
		if (countPlayerPieces[2] < FLY_MODE_LIMIT) {
			countKF = (FLY_MODE_LIMIT - countPlayerPieces[2]) * KILL_WEIGHT;
		}
		value = value + countKF - countKS;
		value = value + (countPlayerPieces[1] - countPlayerPieces[2]) * 20;
	}
	//int value = countPlayerPieces[1] - countPlayerPieces[2];
	return value;
}

int Board::evaluating(bool flyEvalMode,bool debug)
{
	if (boardState == ENDDING)
		return (countPlayerPieces[1] < 4) ? -EVAL_ENDDING : EVAL_ENDDING;
	int valueFirst = evalMethodFirst(flyEvalMode, debug);
	return valueFirst;
}





int drawEval(int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2]) {
	std::cout << "\n";
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++) {
			//std::cout << std::setw(DRAW_SIZE) << evalFirst[i][j];
			if (evalFirst[i][j] == 0)
				std::cout << ".\t";
			else
				std::cout  << evalFirst[i][j]<< '\t';
		}
		std::cout << "\n";
	}
	return 0;
}


//布局使用，把空白都赋MASK,再将中心两点留出，目的是预测未来的周边扩展评分与中心先手评分
int maskCopy(ChessState target[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState from[BOARD_SIZE + 2][BOARD_SIZE + 2], ChessState maskPlayer) {
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++) {
			target[i][j] = ((from[i][j] == NON_PLAYER) ? maskPlayer : from[i][j]);
		}
	}
	target[BOARD_CENTER + 1][BOARD_CENTER + 1] = NON_PLAYER;
	target[BOARD_CENTER][BOARD_CENTER] = NON_PLAYER;
	return 0;
}

int Board::evalLayoutMethodFirst(bool debug) {
	int countEF = 0, countES = 0;
	ChessState centerPlayer = states[BOARD_CENTER + 1][BOARD_CENTER + 1];
	ChessState centerPlayerOppo = states[BOARD_CENTER][BOARD_CENTER];

	states[BOARD_CENTER + 1][BOARD_CENTER + 1] = SECOND_PLAYER;
	states[BOARD_CENTER][BOARD_CENTER] = SECOND_PLAYER;

	int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	convolution(evalFirst, states, EvalKernelFirst, FIRST_PLAYER);
	countEF = layoutCountScore(evalFirst, states, FIRST_PLAYER);

	if (debug) {
		draw();
		drawEval(evalFirst);
	}

	states[BOARD_CENTER + 1][BOARD_CENTER + 1] = FIRST_PLAYER;
	states[BOARD_CENTER][BOARD_CENTER] = FIRST_PLAYER;

	int evalSecond[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	convolution(evalSecond, states, EvalKernelSecond, SECOND_PLAYER);
	countES = layoutCountScore(evalSecond, states, SECOND_PLAYER);
	if (debug) {
		draw();
		drawEval(evalSecond);
	}

	states[BOARD_CENTER + 1][BOARD_CENTER + 1] = centerPlayer;
	states[BOARD_CENTER][BOARD_CENTER] = centerPlayerOppo;


	//getchar();
	int value = countEF - countES;
	return value;
}

int Board::evalLayoutMethodSecond(bool debug) {
	int EF = 0, ES = 0;
	ChessState maskStates[BOARD_SIZE + 2][BOARD_SIZE + 2] = {};

	int evalFirst[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	maskCopy(maskStates, states, SECOND_PLAYER);
	convolution(evalFirst, maskStates, EvalKernelFirst, FIRST_PLAYER);
	EF = countScore(evalFirst, maskStates, FIRST_PLAYER);

	if (debug) {
		draw();
		drawEval(evalFirst);
	}

	int evalSecond[BOARD_SIZE + 2][BOARD_SIZE + 2] = { 0 };
	maskCopy(maskStates, states, FIRST_PLAYER);
	convolution(evalSecond, maskStates, EvalKernelSecond, SECOND_PLAYER);
	ES = countScore(evalSecond, maskStates, SECOND_PLAYER);
	
	if (debug) {
		draw();
		drawEval(evalSecond);
	}

	return EF - ES;
}



int Board::evalConcentration(int conX,int conY)
{
	int x, y;
	int count[3];
	ChessState tmp;
	for (int i = 0; i < 4; i++) {
		x = conX + trans[i][0];
		y = conY + trans[i][1];
		tmp = getPieceState(x, y);
		count[tmp]+=2;
		x += trans[i][0];
		y += trans[i][1];
		if (getPieceState(x, y) == getOppoPlayer(tmp))
			count[getPieceState(x, y)]++;
	}
	return count[1]-count[2];
}


int Board::layoutEval(double weightFirst,double weightSecond, bool debug)
{
	int valueFirst = 0, valueSecond = 0,valueThrird = 0;
	if(weightFirst>0.02)
		valueFirst = evalLayoutMethodFirst(debug);
	if(weightSecond>0.02)
		valueSecond = evalLayoutMethodSecond(debug);
	valueThrird = evalConcentration(BOARD_CENTER, BOARD_CENTER)+ evalConcentration(BOARD_CENTER -1, BOARD_CENTER -1);
	double value = weightFirst * valueFirst + weightSecond * valueSecond+EVAL_LAYOUT_CONCENTRATION*valueThrird;
	return value;
}

ChessState Board::gameEndding()
{
	if (countPlayerPieces[1] < 4)
		return SECOND_PLAYER;
	else if (countPlayerPieces[2] < 4)
		return FIRST_PLAYER;
	return NON_PLAYER;
}

std::ostream & operator<<(std::ostream & cout, Board & board)
{
	cout << "当前场上棋子有" << board.countPieces << "个" << std::endl;
	cout << std::setw(DRAW_SIZE) << ' ';
	for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++)
	{
		cout << std::setw(DRAW_SIZE) << j-1+ROW_OUTPUT_ADJUST;
	}
	cout << std::endl;
	for (int i = BOARD_FOR_START; i < BOARD_FOR_END; i++) {
		cout << std::setw(DRAW_SIZE) << char('A' + i - 1);
		for (int j = BOARD_FOR_START; j < BOARD_FOR_END; j++)
		{
			cout << std::setw(DRAW_SIZE) << stateText[board.states[i][j]];
		}
		cout << std::endl;
	}
	return cout;
}
