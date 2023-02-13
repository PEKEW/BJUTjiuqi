#pragma once

#include "Board.h"
#include <vector>

/*
生成可用于迭代的moves，兼顾排序功能
*/
class MoveMaker
{
public:
	//移动状态 STEP,JUMP,FLY
	enum MoveState
	{
		STEP,
		JUMP,
		FLY
	};
	//方向 UP，LEFT，DOWN，RIGHT,ILLEGAL
	enum Direction
	{
		UP,
		LEFT,
		DOWN,
		RIGHT,
		UNDO,
		ILLEGAL
	};
	static const Direction DIRECTIONS[4];
	struct HalfMove
	{
		int x;
		int y;

		int xe, ye;//飞子终点，state==fly时使用
		MoveState state;
		std::vector<Direction> dirList;//方向组
		int captureMax;//成阵提子数量
		//int JumpCap=-1;//跳跃提子的数量
		int getCaptureJump() const{ 
			return (state==JUMP) ? dirList.size() : 0; 
		}
		//返回结果实际是 this>cmp ，这样做是为了方便std::sort
		bool operator< (const HalfMove &cmp)  const {
			int a = captureMax + getCaptureJump();
			int b = cmp.captureMax + cmp.getCaptureJump();
			if (a == b)
				return captureMax > cmp.captureMax;
			return a > b;

		}

	};
private:
	ChessState player;//本方棋子状态
	ChessState oppo;//对方棋子状态
	Board board;//可变Board，用于深度搜索
	Board conBoard;//永久不变，用于预测提取子

	std::vector<HalfMove> HalfMoves;//全部的halfMove
	bool isMaked=false;

	HalfMove iter;//当前的halfmove，保存一系列的状态,用于迭代深度搜索建立halfmove队列
	std::vector<Direction> direcStack;//方向栈，用于迭代深度搜索

	/*
	用于支持getFirstMove(),getNextMove(),isEndding()三个函数，
	保存当前迭代的状态
	*/
	struct MovesIter {
		int index;//HalfMoves中当前的halfmove坐标，index=0 - (halfmoves.size()-1)
		int posInd[20] = { 0 };//棋子坐标，需要转为xy使用，最多支持20个提子
	} moveIter;
	

	
	
	//所有对方棋子，带有评价值，用于排序后加速ab搜索
	std::vector<PiecePosition> oppoPieces;
	//生成OppoPieces；
	int createOppoPieces();
	bool hasCreOP=false;
	bool isFly;//是飞子一方吗？
	bool isDomine;//是强势方吗？
	JiuMove Iter2JiuMove();

	/*
	私有iter向指定方向前进一步
	如果能够前进，那么将该步计入iter，存储至HalfMoves
	如果不能够前进，则什么也不发生
	*/
	int iterMoveDirec(Direction direc,int &x, int &y);//
	int iterUndo(int &x, int &y);//私有iter撤回一步
	int PieceHalfMoves(int x,int y);//针对单个棋子的HalfMove生成
	int PieceHalfFlyMoves(int, int y);//针对单个棋子的HalfFlyMove生成
public:
	int empty() { return HalfMoves.empty(); }
	int HalfCounts() { return HalfMoves.size(); }
	int makeHalfMoves();//生成HalfMoves，如果已经生成，无事发生
	JiuMove getFirstMove();//返回第一个MOVE
	JiuMove getNextMove();//返回下一个MOVE
	bool isEndding();//迭代终点，判断MOVE是否到头
	int writeHalfMoves();//输出HalfMove，测试内部函数
	MoveMaker(Board board,ChessState player);
	~MoveMaker();
};

class LayoutPositionMaker {
public:
	LayoutPositionMaker(Board &board) :board(board) {
		NonPieces.reserve(BOARD_SIZE*BOARD_SIZE);
		makeNonPieces();
	}
	~LayoutPositionMaker() {}

	Board &board;
	std::vector<PiecePosition> NonPieces;
	PiecePosition &operator[](int i); 
	int makeNonPieces();
	size_t size() { return NonPieces.size(); }
};