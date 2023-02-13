#pragma once
#include "Board.h"

constexpr bool AI_OUTPUT = false;//AI������Ϣ���
constexpr clock_t MAX_MOVE_TIME = 5000;//�������ʱ�� ms
constexpr clock_t MAX_LAYOUT_TIME = 1000;//��󲼾�����ʱ�� ms
constexpr int AI_MAX_DEPTH = 15;//������

class Minimax
{
	
public:
	static clock_t maxTime;//�����ʱ��

	bool useHash = false;//ʹ���û���
	bool useFlyEval = false;//ʹ�÷�������
	Minimax(Board board,ChessState player, std::vector<HashType> & gameHistoryHash);
	Minimax(Board board, ChessState player, std::vector<HashType> & gameHistoryHash, bool useHash, bool useFlyEval);
	//alphaBeta����,depth�Ǳ��������������
	JiuMove ABsearch();
	~Minimax();
private:
	Board rootBoard;//��ʼ����״̬
	ChessState player;//���ڵ�������

	JiuMove bestMoveFirst;//��ǰ���Move


	bool output = AI_OUTPUT;

	const std::vector<HashType> & gameHistoryHash;//��Ϸ�ڲ���ʷHashֵ
	std::vector<HashType> historyHash= std::vector<HashType>(AI_MAX_DEPTH+1);//��¼֮ǰ����hashֵ�������ظ����

	int bestEval;

	int maxDepth=AI_MAX_DEPTH;//�趨����������
	int dep;
	int depWeight;

	int countTrans;//ͳ���û������д���
	int countEval;//ͳ�����۴���
	int countNewTrans;//ͳ���û����滻����
	int resetCounts() {
		countTrans = 0;
		countEval = 0;
		countNewTrans = 0;
		return 0;
	}
	
	clock_t mem_start;


	//���ڵ��ab��������Ҫ����һ�����е�move��Ϊ����
	JiuMove AlphaBetaFirst(int alpha, int beta, int depth, Board &rBoard);
	int AlphaBeta(int alpha, int beta, int depth, Board &rBoard, ChessState nplayer);

	const int repeatedDetection(Board& board,int depth);//�ظ�����̽��
};

class LayoutMinimax {
public:
	static clock_t maxTime;


	Board rootBoard;


	clock_t mem_start;

	LayoutMinimax(Board board, ChessState player,bool useHash=false,int evalRule =0);
	~LayoutMinimax();

	//alphaBeta����,depth�Ǳ��������������
	int AlphaBeta(int alpha, int beta, int depth, Board &rBoard, ChessState nplayer);
	//���ڵ��ab��������Ҫ����һ�����е�move��Ϊ����
	JiuMove AlphaBetaFirst(int alpha, int beta, int depth, Board &rBoard);
	JiuMove ABsearch();


	ChessState player;

	int evalRule;//ʹ�õڼ������۹��򣬿���չ������BvB

	double rulesWeight[2] = { 1,0 };//���۹���Ȩ��
	bool weightAdjust = true;//�״����۵�������

	int bestEval;

	int maxDepth = AI_MAX_DEPTH;//�趨����������
	int dep;
	int depWeight;

	bool output = AI_OUTPUT;
	bool useHash = false;

	int countTrans;//ͳ���û������д���
	int countEval;//ͳ�����۴���
	int countNewTrans;//ͳ���û����滻����


	PiecePosition bestPosFirst;


	int resetCounts() {
		countTrans = 0;
		countEval = 0;
		countNewTrans = 0;
		return 0;
	}
};