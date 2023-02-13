#include "pch.h"
#include "GongJiuPlayer.h"
#include "Minimax.h"
#include "Game.h"

JiuMove GongJiuPlayer::getMove(Board & board, JiuMove lastMove)
{
	if(DRAW_BOARD_FLAG)
		board.drawWithMove(lastMove);
	JiuMove ret;
	if (board.boardState == Board::LAYOUT) {
		if (board.countBlankPieces() > (BOARD_SIZE*BOARD_SIZE - 2)) {
			if (player == FIRST_PLAYER) {
				return JiuMove::firstJiuMove();
				//�˴���bug������õ������֣�Ȼ��ɾ�����Ե��Ӳ����Լ����ӷ�������
				//���Ի����޸�������8��8���ж��������ǷǷ��ģ�����game������һ���µ��ж��������ѭ��
			}
			else {
				return JiuMove::secondJiuMove(lastMove);
			}
		}
		LayoutMinimax lm(board, player, false, 0);
		ret = lm.ABsearch();
	}
	else {
		Minimax m(board, player, game.getReplayHashs(), false, true);
		ret = m.ABsearch();
	}

	std::cout << "��ǰ����߲�Ϊ: " << ret << std::endl<<std::endl;
	return ret;
}

JiuMove NoHashGongJiuPlayer::getMove(Board & board, JiuMove lastMove)
{
	board.drawWithMove(lastMove);
	JiuMove ret;
	if (board.boardState == Board::LAYOUT) {
		if (board.countBlankPieces() > (BOARD_SIZE*BOARD_SIZE - 2)) {
			if (player == FIRST_PLAYER) {
				return JiuMove::firstJiuMove();
			}
			else {
				return JiuMove::secondJiuMove(lastMove);
			}
		}
		LayoutMinimax lm(board, player);
		ret = lm.ABsearch();
	}
	else {
		Minimax m(board, player,game.getReplayHashs(),false,false);
		ret = m.ABsearch();
	}

	std::cout << "��ǰ����߲�Ϊ: " << ret << std::endl;
	return ret;
}

JiuMove flyEvalGongJiuPlayer::getMove(Board & board, JiuMove lastMove)
{
	board.drawWithMove(lastMove);
	JiuMove ret;
	if (board.boardState == Board::LAYOUT) {
		if (board.countBlankPieces() > (BOARD_SIZE*BOARD_SIZE - 2)) {
			if (player == FIRST_PLAYER) {
				return JiuMove::firstJiuMove();
			}
			else {
				return JiuMove::secondJiuMove(lastMove);
			}
		}
		LayoutMinimax lm(board, player,false);
		ret = lm.ABsearch();
	}
	else {
		Minimax m(board, player, game.getReplayHashs(),false,true);
		ret = m.ABsearch();
	}

	std::cout << "��ǰ����߲�Ϊ: " << ret << std::endl;
	return ret;
}

WeightEvalGongJiuPlayerFirst::WeightEvalGongJiuPlayerFirst(Game& game, ChessState p, int rules)
	:Player(game, p, ("���ֹ��򹤾�" + std::to_string(rules))), rules(rules)
{
	
}

JiuMove WeightEvalGongJiuPlayerFirst::getMove(Board & board, JiuMove lastMove)
{
	if(DRAW_BOARD_FLAG)
		board.drawWithMove(lastMove);
	JiuMove ret;
	if (board.boardState == Board::LAYOUT) {
		if (board.countBlankPieces() > (BOARD_SIZE*BOARD_SIZE - 2)) {
			if (player == FIRST_PLAYER) {
				return JiuMove::firstJiuMove();
			}
			else {
				return JiuMove::secondJiuMove(lastMove);
			}
		}
		LayoutMinimax lm(board, player,false,rules);
		lm.useHash = false;
		ret = lm.ABsearch();
	}
	else {
		Minimax m(board, player, game.getReplayHashs(), false, true);
		m.useHash = false;
		ret = m.ABsearch();
	}

	std::cout << "��ǰ����߲�Ϊ: " << ret << std::endl;
	return ret;
}
