#pragma once
#include "Player.h"
class GongJiuPlayer :
	public Player
{
public:
	GongJiuPlayer(Game& game, ChessState p):Player(game,p,"����"){}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~GongJiuPlayer(){}
};
//��ʹ���û����ѳ�ΪĬ������
class NoHashGongJiuPlayer :public Player {
public:
	NoHashGongJiuPlayer(Game& game, ChessState p) :Player(game, p, "���û�����") {}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~NoHashGongJiuPlayer() { }
};
//ʹ�÷��ӽ׶��������ۣ��ѳ�ΪĬ������
class flyEvalGongJiuPlayer :public Player {
public:
	flyEvalGongJiuPlayer(Game& game, ChessState p) :Player(game, p, "�������۹���") {}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~flyEvalGongJiuPlayer() {}
};

//ʹ�ò�ͬ����Ȩ�أ�rules��0-3��4�����ۣ�Ĭ�����ò���rules0
class WeightEvalGongJiuPlayerFirst :public Player {
public:
	WeightEvalGongJiuPlayerFirst(Game& game, ChessState p, int rules);
	JiuMove getMove(Board&board, JiuMove lastMove);
	int rules;
	~WeightEvalGongJiuPlayerFirst() {}
};
