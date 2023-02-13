#pragma once

#include "header.h"
#include <iostream>
#include <iomanip>


//2*2����ƥ���
constexpr int EvalKernelFirst[81][4] =
{ {0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,1,0,0},{4,4,4,4},{0,1,0,0},{4,4,4,4},{192,16,16,16},{4,4,4,4},{0,1,0,0},{4,4,4,4},{0,1,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,192,16,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,16,192,16},{4,4,4,4},{16,16,16,192},{64,64,64,64},{16,16,16,192},{4,4,4,4},{16,16,192,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{4,4,4,4},{16,192,16,16},{4,4,4,4},{1,0,0,0},{4,4,4,4},{1,0,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,1,0,0},{4,4,4,4},{0,1,0,0},{4,4,4,4},{192,16,16,16},{4,4,4,4},{0,1,0,0},{4,4,4,4},{0,1,0,0},{0,0,0,0},{0,0,0,1},{0,0,0,0},{0,0,1,0},{4,4,4,4},{0,0,1,0},{0,0,0,0},{0,0,0,1},{0,0,0,0} };
constexpr int EvalKernelSecond[81][4] =
{ {0, 0, 0, 0}, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 192,16,16,16 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,1 }, { 0,0,1,0 }, { 0,0,1,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 0,1,0,0 }, { 0,1,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 192,16,16,16 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,192,16,16 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 1,0,0,0 }, { 1,0,0,0 }, { 4,4,4,4 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,192,16,16 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,16,192,16 }, { 4,4,4,4 }, { 4,4,4,4 }, { 16,16,192,16 }, { 16,16,16,192 }, { 16,16,16,192 }, { 64,64,64,64 } };

/*
Board��
������Ϸ�ڲ��߼������浱ǰ���״̬����һ������̨��������Ļ�ͼ����
�ṩһ��ı䵱ǰ��ֵĽӿ�

�õ������ж�
ִ�п����ж�
�ж�ʤ��
���þ���
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
	/*����״̬������ÿ��λ�õ����ӹ������ޣ����1�����2
	���ж�����2����ԭ��������Χ��һȦ�������ж�Խ������
	xxxxx
	x***x
	x***x
	x***x
	xxxxx
	*/
	ChessState states[BOARD_SIZE + 2][BOARD_SIZE + 2] = { NON_PLAYER };
	//���ּӺ������е���������
	int countPieces = 0;
	//�ա����֡����֣����ߵ���������
	int countPlayerPieces[3] = { BOARD_SIZE*BOARD_SIZE,0,0 };
public:
	Board();
	~Board();


	static bool weakNotJumpSingle;//���Ʒ����ܵ�����Ĭ��Ϊtrue

	//�����ж�
	bool outSide(int x,int y) {
		return states[x + 1][y + 1] == ILLEGAL_PLAYER;
	}
	//�����������
	friend std::ostream & operator<<(std::ostream &cout, Board &board);
	int draw() { std::cout << (*this); return 0; }
	//����һ����ʾ���������
	int drawWithMove(JiuMove move);
	
	//��ǰ����Hashֵ
	HashType ZbHash=0;
	int countBlankPieces() { return countPlayerPieces[0];}
	int getCountPlayerPieces(ChessState player) { return countPlayerPieces[player]; }
	//�����Player�Ƿ��������
	const bool isFlyPlayer(ChessState Player) {
		return countPlayerPieces[Player] < FLY_MODE_LIMIT;
	}
	ChessState getPieceState(int x, int y) { 
		return states[x + 1][y + 1];
	}
	bool isState(int x, int y, ChessState target) {
		return states[x+1][y+1] == target;
	}
	//ָ��λ��������Ϊ��
	int resetPieceState(int x, int y);
	//ָ��λ��������Ϊָ���������
	int addPieceState(int x, int y, ChessState state);
	int setPieceState(int x, int y, ChessState state) {
		if (state == NON_PLAYER)
			resetPieceState(x, y);
		else
			addPieceState(x, y, state);
		return 0;
	}
	
	/*
	ִ��jiumove
	ret<0��������Ƿ��������/�Ἲ����/��Ƿ���
	��ִ�н׶��жϺϷ��Ը���
	*/
	const int doMoves(JiuMove move);
	int removeCenterPieces();
	//Ԥ���ƶ�����Ҫ��֧��MoveMaker�����ɵ�HalfMove�������ƶ��������Է�������
	int preMove(int xs, int ys, int xe, int ye);
	//�������ۺ���1�����ںڰ��������ֱ���оֲ����������ۼ��������õ����߷�����ʹ�����1����-���2�����õ����ؽ��
	int evalMethodFirst(bool flyEvalMode,bool debug);
	//�����ľ������ۺ���������ڣ������ڸú����е����������溯������ͨ�����ò�ͬȨ�صõ�����ֵ����Ϊ�˴�ֻ����һ�����ۺ���������û�м�Ȩ
	int evaluating(bool flyEvalMode =true ,bool debug = false);

	//��������1:
	int evalLayoutMethodFirst(bool debug);
	//��������2:
	int evalLayoutMethodSecond(bool debug);
	int evalConcentration(int conX, int conY);//����Ŀ����ʼ�����Χ�����Ӳ�ֵ

	//�����Ĳ��־������ۺ���������ڣ�������ͬ��
	int layoutEval(double weightFirst=1, double weightSecond=0, bool debug=false);
	//���ػ�ʤ�������û�л�ʤ�ߣ�����NON_PLAYER
	ChessState gameEndding();
};

