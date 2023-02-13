#pragma once
#include "Board.h"


class Game;
class Player
{
public:
	ChessState player=NON_PLAYER;
	std::string name;
	Game& game;

	Player(Game& game, ChessState p) :player(p), name("UNKOWN_PLAYER"), game(game){}
	Player(Game& game,ChessState p,std::string name):player(p), name(name), game(game){}

	virtual JiuMove getMove(Board& board,JiuMove lastMove)=0;
	virtual ~Player()=default;
};

