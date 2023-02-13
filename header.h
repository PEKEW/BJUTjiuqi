#pragma once
#include <vector>
#include <array>
#include <iostream>
enum ChessState
{
	NON_PLAYER = 0,//空子
	FIRST_PLAYER = 1,//先手玩家
	SECOND_PLAYER = 2,//后手玩家
	ILLEGAL_PLAYER = -1//非法玩家
};

inline ChessState getOppoPlayer(ChessState our) {
	if (our == FIRST_PLAYER)
		return SECOND_PLAYER;
	if (our == SECOND_PLAYER)
		return FIRST_PLAYER;
	return ILLEGAL_PLAYER;
}
/*
move类型
*/
enum MoveState
{
	ADD_PIECE, //布局落子
	DROP_PIECE,//开局提子
	STEP_PIECE,//移动行棋
	JUMP_PIECE,//跳子移动
	FLY_PIECE,//终局飞子
	ERROR_PIECE,//非法移动，用于返回空值
};

const char stateText[] = { '.','*','o','!' };//棋盘输出符号
constexpr int BOARD_SIZE = 14;	//棋盘大小
constexpr int BOARD_PIECE_LIMIT = BOARD_SIZE * BOARD_SIZE;//棋盘能容纳的最大值
constexpr int FLY_MODE_LIMIT = BOARD_SIZE+1;//飞子大小限制

constexpr int BOARD_CENTER = BOARD_SIZE / 2;//棋盘中心坐标较大处

constexpr int BOARD_FOR_START = 1;//用于Fot循环中，Board中真正棋盘的起点
constexpr int BOARD_FOR_END = BOARD_SIZE+1;//用于Fot循环中，Board中真正棋盘的终点
constexpr int DRAW_SIZE = 3;	//输出显示，每个棋子横向所占空间
constexpr bool DRAW_BOARD_FLAG = true;//电脑是否在行动前先输出当前局面

constexpr int EVAL_ENDDING = 1000000;//终局评分奖励
constexpr int KILL_WEIGHT = 10000;//击杀飞子奖励
constexpr int EVAL_DALIAN = 3000;//褡裢奖励
constexpr int EVAL_LAYOUT_CONCENTRATION = 10;//棋子靠近中央的奖励权重

constexpr int REPEATED_DRAW_ROUNDS = 4;//在指定轮数内重复判和棋
/*
最大不吃子回合数，用于平局检测中。
真实棋局没有平局规则，可以协商平局。
但人工智能对弈有时会陷入局部最优解，尤其是相同的AI，会陷入为了不降低评分而分别走闲招的情况，
此时他们之间的bo10往往这一局下到天荒地老也下不完，为了在这种情况下，重新开始新的一局，加入此规则
*/
constexpr int PEACE_ROUNDS = 15;
typedef std::vector<std::array<int, 2 >> Positions;

//四个方向的数组，可以通过简单的for遍历一个棋子的四个方向
constexpr int trans[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };
constexpr int jTrans[4][2] = { {-2,0},{0,-2},{2,0},{0,2} };

//横向调整坐标轴，为了适应平台的棋盘,0是默认，1是挪动1,原本的A0会变成A1，AD会变成AE
constexpr int ROW_OUTPUT_ADJUST = 1;

struct JiuMove
{
	ChessState player = NON_PLAYER;
	//move状态
	MoveState moveState=STEP_PIECE;

	//棋子起始坐标
	int xStartPosition = -1;
	int yStartPosition = -1;

	//棋子结束坐标
	int xEndPosition = -1;
	int yEndPosition = -1;

	//移动跳过棋子的数量，add函数自动调整
	int jumpCaptureCount = 0;
	//按规则提出的棋子的数量，add函数自动调整
	int captureCount = 0;
	//跳过棋子的坐标位置，本棋子的位置记录，用于输出棋谱
	Positions jumpPositions;
	//提出棋子的坐标，包括成方吃子与跳吃子,用于改变board
	Positions capturePositions;

	JiuMove() :JiuMove(-1, -1, -1, -1) {  }
	void initPos(int xStart, int yStart, int xEnd, int yEnd);
	JiuMove(int xStart, int yStart, int xEnd, int yEnd);
	//添加跳跃中途点，是我方子的坐标
	int addJumpPos(int x,int y);
	//添加被吃子坐标，是对方的坐标，跳吃与成方吃均调用此函数
	int addCapPos(int x,int y);

	//返回一个非法JiuMove，用于各类需要JiuMove返回值，却出现error的情况
	static JiuMove errorJiuMove() {
		JiuMove err;
		err.moveState = ERROR_PIECE;
		return err;
	}
	//返回一个中心坐标点，例如14*14棋盘，返回一个（7,7），即h8
	static JiuMove firstJiuMove() {
		JiuMove ret(BOARD_CENTER, BOARD_CENTER, BOARD_CENTER, BOARD_CENTER);
		ret.player = FIRST_PLAYER;
		ret.moveState = ADD_PIECE;
		return ret;
	}
	//输入对方选择的第一步，返回下在另一个中心点的JiuMove
	static JiuMove secondJiuMove(JiuMove first) {
		int position = BOARD_CENTER;
		if (first.xStartPosition == position)
			position = position-1;
		JiuMove ret(position,position,position,position);
		ret.player = SECOND_PLAYER;
		ret.moveState = ADD_PIECE;
		return ret;
	}
	//重载输出函数
	friend std::ostream& operator<<(std::ostream &out, JiuMove &m);
};

//历史表
extern double historyTable[BOARD_SIZE][BOARD_SIZE];
//将启发性的提子计入历史表,权值为搜索深度的平方
int addHistory(JiuMove m, int weight);
//历史表权重衰退
int declineHistory();


typedef unsigned long long HashType;
struct TransTable {
	HashType zobrist;//原hash值
	int value;//当前评价值
	int depth;//当前深度
};

//利用了历史表信息的坐标比较函数
struct PiecePosition {
	std::array<int, 2> pos;
	PiecePosition(int x, int y) :
		pos({ x,y })
	{}
	PiecePosition() :PiecePosition(0, 0) {}
	bool operator<(const PiecePosition & rhs) const {
		return (historyTable[pos[0]][pos[1]] > historyTable[rhs.pos[0]][rhs.pos[1]]);
	}
};