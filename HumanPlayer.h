#pragma once
#include "Player.h"
class HumanPlayer :
	public Player
{
public:
	HumanPlayer(Game& game, ChessState p);
	double timeWeight = 1;//用于调整搜索时长
	JiuMove getMove(Board& board,JiuMove lastMove);
	~HumanPlayer();
};

