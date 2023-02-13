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
				//此处有bug，如果让电脑先手，然后删掉电脑的子并把自己的子放在上面
				//电脑会无限给出落子8，8的行动，但这是非法的，所以game会申请一个新的行动，造成死循环
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

	std::cout << "当前最佳走步为: " << ret << std::endl<<std::endl;
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

	std::cout << "当前最佳走步为: " << ret << std::endl;
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

	std::cout << "当前最佳走步为: " << ret << std::endl;
	return ret;
}

WeightEvalGongJiuPlayerFirst::WeightEvalGongJiuPlayerFirst(Game& game, ChessState p, int rules)
	:Player(game, p, ("布局规则工久" + std::to_string(rules))), rules(rules)
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

	std::cout << "当前最佳走步为: " << ret << std::endl;
	return ret;
}
