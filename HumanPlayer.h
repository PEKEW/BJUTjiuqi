#pragma once
#include "Player.h"
class HumanPlayer :
	public Player
{
public:
	HumanPlayer(Game& game, ChessState p);
	double timeWeight = 1;//���ڵ�������ʱ��
	JiuMove getMove(Board& board,JiuMove lastMove);
	~HumanPlayer();
};

