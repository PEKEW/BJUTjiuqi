#pragma once
#include "Board.h"

constexpr bool AI_OUTPUT = false;//AI搜索信息输出
constexpr clock_t MAX_MOVE_TIME = 5000;//最大搜索时长 ms
constexpr clock_t MAX_LAYOUT_TIME = 1000;//最大布局搜索时长 ms
constexpr int AI_MAX_DEPTH = 15;//最大深度

class Minimax
{
	
public:
	static clock_t maxTime;//最长搜索时间

	bool useHash = false;//使用置换表
	bool useFlyEval = false;//使用飞子评价
	Minimax(Board board,ChessState player, std::vector<HashType> & gameHistoryHash);
	Minimax(Board board, ChessState player, std::vector<HashType> & gameHistoryHash, bool useHash, bool useFlyEval);
	//alphaBeta搜索,depth是本次搜索的最深度
	JiuMove ABsearch();
	~Minimax();
private:
	Board rootBoard;//初始搜索状态
	ChessState player;//根节点搜索方

	JiuMove bestMoveFirst;//当前最佳Move


	bool output = AI_OUTPUT;

	const std::vector<HashType> & gameHistoryHash;//游戏内部历史Hash值
	std::vector<HashType> historyHash= std::vector<HashType>(AI_MAX_DEPTH+1);//记录之前步的hash值，用于重复检测

	int bestEval;

	int maxDepth=AI_MAX_DEPTH;//设定最大搜索深度
	int dep;
	int depWeight;

	int countTrans;//统计置换表命中次数
	int countEval;//统计评价次数
	int countNewTrans;//统计置换表替换次数
	int resetCounts() {
		countTrans = 0;
		countEval = 0;
		countNewTrans = 0;
		return 0;
	}
	
	clock_t mem_start;


	//根节点的ab搜索，需要给定一个可行的move作为返回
	JiuMove AlphaBetaFirst(int alpha, int beta, int depth, Board &rBoard);
	int AlphaBeta(int alpha, int beta, int depth, Board &rBoard, ChessState nplayer);

	const int repeatedDetection(Board& board,int depth);//重复局面探测
};

class LayoutMinimax {
public:
	static clock_t maxTime;


	Board rootBoard;


	clock_t mem_start;

	LayoutMinimax(Board board, ChessState player,bool useHash=false,int evalRule =0);
	~LayoutMinimax();

	//alphaBeta搜索,depth是本次搜索的最深度
	int AlphaBeta(int alpha, int beta, int depth, Board &rBoard, ChessState nplayer);
	//根节点的ab搜索，需要给定一个可行的move作为返回
	JiuMove AlphaBetaFirst(int alpha, int beta, int depth, Board &rBoard);
	JiuMove ABsearch();


	ChessState player;

	int evalRule;//使用第几条评价规则，可拓展，用于BvB

	double rulesWeight[2] = { 1,0 };//评价规则权重
	bool weightAdjust = true;//首次评价调整规则

	int bestEval;

	int maxDepth = AI_MAX_DEPTH;//设定最大搜索深度
	int dep;
	int depWeight;

	bool output = AI_OUTPUT;
	bool useHash = false;

	int countTrans;//统计置换表命中次数
	int countEval;//统计评价次数
	int countNewTrans;//统计置换表替换次数


	PiecePosition bestPosFirst;


	int resetCounts() {
		countTrans = 0;
		countEval = 0;
		countNewTrans = 0;
		return 0;
	}
};