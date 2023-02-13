#pragma once

#include "Board.h"
#include <vector>

/*
���ɿ����ڵ�����moves�����������
*/
class MoveMaker
{
public:
	//�ƶ�״̬ STEP,JUMP,FLY
	enum MoveState
	{
		STEP,
		JUMP,
		FLY
	};
	//���� UP��LEFT��DOWN��RIGHT,ILLEGAL
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

		int xe, ye;//�����յ㣬state==flyʱʹ��
		MoveState state;
		std::vector<Direction> dirList;//������
		int captureMax;//������������
		//int JumpCap=-1;//��Ծ���ӵ�����
		int getCaptureJump() const{ 
			return (state==JUMP) ? dirList.size() : 0; 
		}
		//���ؽ��ʵ���� this>cmp ����������Ϊ�˷���std::sort
		bool operator< (const HalfMove &cmp)  const {
			int a = captureMax + getCaptureJump();
			int b = cmp.captureMax + cmp.getCaptureJump();
			if (a == b)
				return captureMax > cmp.captureMax;
			return a > b;

		}

	};
private:
	ChessState player;//��������״̬
	ChessState oppo;//�Է�����״̬
	Board board;//�ɱ�Board�������������
	Board conBoard;//���ò��䣬����Ԥ����ȡ��

	std::vector<HalfMove> HalfMoves;//ȫ����halfMove
	bool isMaked=false;

	HalfMove iter;//��ǰ��halfmove������һϵ�е�״̬,���ڵ��������������halfmove����
	std::vector<Direction> direcStack;//����ջ�����ڵ����������

	/*
	����֧��getFirstMove(),getNextMove(),isEndding()����������
	���浱ǰ������״̬
	*/
	struct MovesIter {
		int index;//HalfMoves�е�ǰ��halfmove���꣬index=0 - (halfmoves.size()-1)
		int posInd[20] = { 0 };//�������꣬��ҪתΪxyʹ�ã����֧��20������
	} moveIter;
	

	
	
	//���жԷ����ӣ���������ֵ��������������ab����
	std::vector<PiecePosition> oppoPieces;
	//����OppoPieces��
	int createOppoPieces();
	bool hasCreOP=false;
	bool isFly;//�Ƿ���һ����
	bool isDomine;//��ǿ�Ʒ���
	JiuMove Iter2JiuMove();

	/*
	˽��iter��ָ������ǰ��һ��
	����ܹ�ǰ������ô���ò�����iter���洢��HalfMoves
	������ܹ�ǰ������ʲôҲ������
	*/
	int iterMoveDirec(Direction direc,int &x, int &y);//
	int iterUndo(int &x, int &y);//˽��iter����һ��
	int PieceHalfMoves(int x,int y);//��Ե������ӵ�HalfMove����
	int PieceHalfFlyMoves(int, int y);//��Ե������ӵ�HalfFlyMove����
public:
	int empty() { return HalfMoves.empty(); }
	int HalfCounts() { return HalfMoves.size(); }
	int makeHalfMoves();//����HalfMoves������Ѿ����ɣ����·���
	JiuMove getFirstMove();//���ص�һ��MOVE
	JiuMove getNextMove();//������һ��MOVE
	bool isEndding();//�����յ㣬�ж�MOVE�Ƿ�ͷ
	int writeHalfMoves();//���HalfMove�������ڲ�����
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