#pragma once
#include <vector>
#include <array>
#include <iostream>
enum ChessState
{
	NON_PLAYER = 0,//����
	FIRST_PLAYER = 1,//�������
	SECOND_PLAYER = 2,//�������
	ILLEGAL_PLAYER = -1//�Ƿ����
};

inline ChessState getOppoPlayer(ChessState our) {
	if (our == FIRST_PLAYER)
		return SECOND_PLAYER;
	if (our == SECOND_PLAYER)
		return FIRST_PLAYER;
	return ILLEGAL_PLAYER;
}
/*
move����
*/
enum MoveState
{
	ADD_PIECE, //��������
	DROP_PIECE,//��������
	STEP_PIECE,//�ƶ�����
	JUMP_PIECE,//�����ƶ�
	FLY_PIECE,//�վַ���
	ERROR_PIECE,//�Ƿ��ƶ������ڷ��ؿ�ֵ
};

const char stateText[] = { '.','*','o','!' };//�����������
constexpr int BOARD_SIZE = 14;	//���̴�С
constexpr int BOARD_PIECE_LIMIT = BOARD_SIZE * BOARD_SIZE;//���������ɵ����ֵ
constexpr int FLY_MODE_LIMIT = BOARD_SIZE+1;//���Ӵ�С����

constexpr int BOARD_CENTER = BOARD_SIZE / 2;//������������ϴ�

constexpr int BOARD_FOR_START = 1;//����Fotѭ���У�Board���������̵����
constexpr int BOARD_FOR_END = BOARD_SIZE+1;//����Fotѭ���У�Board���������̵��յ�
constexpr int DRAW_SIZE = 3;	//�����ʾ��ÿ�����Ӻ�����ռ�ռ�
constexpr bool DRAW_BOARD_FLAG = true;//�����Ƿ����ж�ǰ�������ǰ����

constexpr int EVAL_ENDDING = 1000000;//�վ����ֽ���
constexpr int KILL_WEIGHT = 10000;//��ɱ���ӽ���
constexpr int EVAL_DALIAN = 3000;//���ͽ���
constexpr int EVAL_LAYOUT_CONCENTRATION = 10;//���ӿ�������Ľ���Ȩ��

constexpr int REPEATED_DRAW_ROUNDS = 4;//��ָ���������ظ��к���
/*
��󲻳��ӻغ���������ƽ�ּ���С�
��ʵ���û��ƽ�ֹ��򣬿���Э��ƽ�֡�
���˹����ܶ�����ʱ������ֲ����Ž⣬��������ͬ��AI��������Ϊ�˲��������ֶ��ֱ������е������
��ʱ����֮���bo10������һ���µ���ĵ���Ҳ�²��꣬Ϊ������������£����¿�ʼ�µ�һ�֣�����˹���
*/
constexpr int PEACE_ROUNDS = 15;
typedef std::vector<std::array<int, 2 >> Positions;

//�ĸ���������飬����ͨ���򵥵�for����һ�����ӵ��ĸ�����
constexpr int trans[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };
constexpr int jTrans[4][2] = { {-2,0},{0,-2},{2,0},{0,2} };

//������������ᣬΪ����Ӧƽ̨������,0��Ĭ�ϣ�1��Ų��1,ԭ����A0����A1��AD����AE
constexpr int ROW_OUTPUT_ADJUST = 1;

struct JiuMove
{
	ChessState player = NON_PLAYER;
	//move״̬
	MoveState moveState=STEP_PIECE;

	//������ʼ����
	int xStartPosition = -1;
	int yStartPosition = -1;

	//���ӽ�������
	int xEndPosition = -1;
	int yEndPosition = -1;

	//�ƶ��������ӵ�������add�����Զ�����
	int jumpCaptureCount = 0;
	//��������������ӵ�������add�����Զ�����
	int captureCount = 0;
	//�������ӵ�����λ�ã������ӵ�λ�ü�¼�������������
	Positions jumpPositions;
	//������ӵ����꣬�����ɷ�������������,���ڸı�board
	Positions capturePositions;

	JiuMove() :JiuMove(-1, -1, -1, -1) {  }
	void initPos(int xStart, int yStart, int xEnd, int yEnd);
	JiuMove(int xStart, int yStart, int xEnd, int yEnd);
	//�����Ծ��;�㣬���ҷ��ӵ�����
	int addJumpPos(int x,int y);
	//��ӱ��������꣬�ǶԷ������꣬������ɷ��Ծ����ô˺���
	int addCapPos(int x,int y);

	//����һ���Ƿ�JiuMove�����ڸ�����ҪJiuMove����ֵ��ȴ����error�����
	static JiuMove errorJiuMove() {
		JiuMove err;
		err.moveState = ERROR_PIECE;
		return err;
	}
	//����һ����������㣬����14*14���̣�����һ����7,7������h8
	static JiuMove firstJiuMove() {
		JiuMove ret(BOARD_CENTER, BOARD_CENTER, BOARD_CENTER, BOARD_CENTER);
		ret.player = FIRST_PLAYER;
		ret.moveState = ADD_PIECE;
		return ret;
	}
	//����Է�ѡ��ĵ�һ��������������һ�����ĵ��JiuMove
	static JiuMove secondJiuMove(JiuMove first) {
		int position = BOARD_CENTER;
		if (first.xStartPosition == position)
			position = position-1;
		JiuMove ret(position,position,position,position);
		ret.player = SECOND_PLAYER;
		ret.moveState = ADD_PIECE;
		return ret;
	}
	//�����������
	friend std::ostream& operator<<(std::ostream &out, JiuMove &m);
};

//��ʷ��
extern double historyTable[BOARD_SIZE][BOARD_SIZE];
//�������Ե����Ӽ�����ʷ��,ȨֵΪ������ȵ�ƽ��
int addHistory(JiuMove m, int weight);
//��ʷ��Ȩ��˥��
int declineHistory();


typedef unsigned long long HashType;
struct TransTable {
	HashType zobrist;//ԭhashֵ
	int value;//��ǰ����ֵ
	int depth;//��ǰ���
};

//��������ʷ����Ϣ������ȽϺ���
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