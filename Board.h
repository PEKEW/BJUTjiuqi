#pragma once

#include "header.h"
#include <iostream>
#include <iomanip>


//2*2评价匹配表
constexpr int EvalKernelFirst[81][4] =
{ {0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,1,0,0},{4,4,4,4},{0,1,0,0},{4,4,4,4},{192,16,16,16},{4,4,4,4},{0,1,0,0},{4,4,4,4},{0,1,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,192,16,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,16,192,16},{4,4,4,4},{16,16,16,192},{64,64,64,64},{16,16,16,192},{4,4,4,4},{16,16,192,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,192,16,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,1,0,0},{4,4,4,4},{0,1,0,0},{4,4,4,4},{192,16,16,16},{4,4,4,4},{0,1,0,0},{4,4,4,4},{0,1,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0} };
constexpr int EvalKernelSecond[81][4] =
{ {0, 0, 0, 0}, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 192,16,16,16 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 192,16,16,16 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,192,16,16 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,192,16,16 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,16,192,16 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,16,192,16 }, { 16,16,16,192 }, { 16,16,16,192 }, { 64,64,64,64 } };

/*
Board类
负责游戏内部逻辑，保存当前棋局状态，有一个控制台向外输出的绘图功能
提供一组改变当前棋局的接口

得到可行行动
执行可行行动
判定胜负
重置局面
*/
class Board
{
public:
	enum BoardState
	{
		LAYOUT,
		MOVE,
		FLY,
		ENDDING
	} boardState = LAYOUT; 
private:
	/*棋盘状态，保存每个位置的棋子归属：无，玩家1，玩家2
	行列都加了2，在原本棋盘外围绕一圈辅助，判断越界条件
	xxxxx
	x***x
	x***x
	x***x
	xxxxx
	*/
	ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2] = { NON_PLAYER };
	//先手加后手所有的棋子数量
	int countPieces = 0;
	//空、先手、后手，三者的棋子数量
	int countPlayerPieces[3] = { BOARD_SIZE*BOARD_SIZE,0,0 };
public:
	Board();
	~Board();


	static bool weakNotJumpSingle;//弱势方不能单跳，默认为true

	//出界判断
	bool outSide(int x,int y) {
		return states[x + 1][y + 1] == ILLEGAL_PLAYER;
	}
	//重载输出函数
	friend std::ostream & operator<<(std::ostream &cout, Board &board);
	int draw() { std::cout << (*this); return 0; }
	//带上一步提示的输出函数
	int drawWithMove(JiuMove move);
	
	//当前局面Hash值
	HashType ZbHash=0;
	int countBlankPieces() { return countPlayerPieces[0];}
	int getCountPlayerPieces(ChessState player) { return countPlayerPieces[player]; }
	//输入的Player是飞子玩家吗？
	const bool isFlyPlayer(ChessState Player) {
		return countPlayerPieces[Player] < FLY_MODE_LIMIT;
	}
	ChessState getPieceState(int x, int y) { 
		return states[x + 1][y + 1];
	}
	bool isState(int x, int y, ChessState target) {
		return states[x+1][y+1] == target;
	}
	//指定位置棋子设为空
	int resetPieceState(int x, int y);
	//指定位置棋子设为指定玩家所有
	int addPieceState(int x, int y, ChessState state);
	int setPieceState(int x, int y, ChessState state) {
		if (state == NON_PLAYER)
			resetPieceState(x, y);
		else
			addPieceState(x, y, state);
		return 0;
	}
	
	/*
	执行jiumove
	ret<0代表操作非法，提空子/提己方子/提非法子
	在执行阶段判断合法性更简单
	*/
	const int doMoves(JiuMove move);
	int removeCenterPieces();
	//预先移动，主要是支持MoveMaker中生成的HalfMove，返回移动后可以提对方几个子
	int preMove(int xs, int ys, int xe, int ye);
	//局面评价函数1，对于黑白两方，分别进行局部评估后再累加起来，得到两边分数，使用玩家1分数-玩家2分数得到返回结果
	int evalMethodFirst(bool flyEvalMode,bool debug);
	//真正的局面评价函数调用入口，可以在该函数中调用其他局面函数，再通过设置不同权重得到最终值，因为此处只用了一个评价函数，所以没有加权
	int evaluating(bool flyEvalMode =true ,bool debug = false);

	//布局评价1:
	int evalLayoutMethodFirst(bool debug);
	//布局评价2:
	int evalLayoutMethodSecond(bool debug);
	int evalConcentration(int conX, int conY);//测试目标起始点的周围的棋子差值

	//真正的布局局面评价函数调用入口，与上面同理
	int layoutEval(double weightFirst=1, double weightSecond=0, bool debug=false);
	//返回获胜方，如果没有获胜者，返回NON_PLAYER
	ChessState gameEndding();
};

