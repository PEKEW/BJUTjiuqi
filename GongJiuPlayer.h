#pragma once
#include "Player.h"
class GongJiuPlayer :
	public Player
{
public:
	GongJiuPlayer(Game& game, ChessState p):Player(game,p,"工久"){}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~GongJiuPlayer(){}
};
//不使用置换表，已成为默认设置
class NoHashGongJiuPlayer :public Player {
public:
	NoHashGongJiuPlayer(Game& game, ChessState p) :Player(game, p, "无置换表工久") {}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~NoHashGongJiuPlayer() { }
};
//使用飞子阶段特殊评价，已成为默认设置
class flyEvalGongJiuPlayer :public Player {
public:
	flyEvalGongJiuPlayer(Game& game, ChessState p) :Player(game, p, "飞子评价工久") {}
	JiuMove getMove(Board&board, JiuMove lastMove);
	~flyEvalGongJiuPlayer() {}
};

//使用不同布局权重，rules从0-3有4种评价，默认配置采用rules0
class WeightEvalGongJiuPlayerFirst :public Player {
public:
	WeightEvalGongJiuPlayerFirst(Game& game, ChessState p, int rules);
	JiuMove getMove(Board&board, JiuMove lastMove);
	int rules;
	~WeightEvalGongJiuPlayerFirst() {}
};
