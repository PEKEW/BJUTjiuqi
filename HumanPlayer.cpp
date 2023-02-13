#include "pch.h"
#include "HumanPlayer.h"
#include "ManualReader.h"
#include "Game.h"
#include "Minimax.h"
#include <iostream>
#include <string>

HumanPlayer::HumanPlayer(Game& game, ChessState p):Player::Player(game,p,"���")
{
}

JiuMove HumanPlayer::getMove(Board & board, JiuMove lastMove)
{
	if (lastMove.moveState == ERROR_PIECE)
		board.draw();
	else
		board.drawWithMove(lastMove);
	std::string s;
	std::cin >> s;

	if (s[0] == '!') {
		if (s[1] == 'r' || s[1]=='R') {
			game.setUndoFlag();
		}
		if (s[1] == 't' || s[1] == 'T') {
			timeWeight -= 0.4;
			if (timeWeight < 0.2)
				timeWeight = 0.2;
			Minimax::maxTime = MAX_MOVE_TIME * timeWeight;
			LayoutMinimax::maxTime = MAX_LAYOUT_TIME * timeWeight;
			std::cout << "����ʱ����٣���ǰΪ" << Minimax::maxTime << "ms," << LayoutMinimax::maxTime << "ms\n";
		}
		if (s[1] == 'y' || s[1] == 'Y') {
			timeWeight += 0.4;
			Minimax::maxTime = MAX_MOVE_TIME * timeWeight;
			LayoutMinimax::maxTime = MAX_LAYOUT_TIME * timeWeight;
			std::cout << "����ʱ�����ӣ���ǰΪ" << Minimax::maxTime << "ms," << LayoutMinimax::maxTime << "ms\n";
		}
		if (s[1] == 'p' || s[1] == 'P') {
			std::cout << "��������ģʽ�������´����ӵ����\n";
			game.setChangeBoardFlag();
		}
		return JiuMove::errorJiuMove();
	}

	if (board.boardState == Board::LAYOUT)
		return ManualReader::Str2JiuMove(s,player,ADD_PIECE);
	return ManualReader::Str2JiuMove(s, player);
}


HumanPlayer::~HumanPlayer()
{
}
