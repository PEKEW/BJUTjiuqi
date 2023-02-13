#include "pch.h"
#include "Minimax.h"
#include "MoveMaker.h"
#include "Zobrist.h"


clock_t Minimax::maxTime = MAX_MOVE_TIME;
clock_t LayoutMinimax::maxTime = MAX_LAYOUT_TIME;


Minimax::Minimax(Board board, ChessState player, std::vector<HashType> & gameHistoryHash):
	rootBoard(board),
	player(player),
	gameHistoryHash(gameHistoryHash)
{

}

Minimax::Minimax(Board board, ChessState player, std::vector<HashType> & gameHistoryHash, bool useHash, bool useFlyEval):
	rootBoard(board),
	player(player),
	gameHistoryHash(gameHistoryHash),
	useHash(useHash),
	useFlyEval(useFlyEval)
{
}

int Minimax::AlphaBeta(int alpha, int beta, int depth,Board & rBoard,ChessState nplayer)
{
	if (depth == 0) {
		int eval=INT_MAX;
		if (useHash) {
			int hashCode = rBoard.ZbHash >> (64 - Zbsize);
			if (zobrist.transTable[hashCode].zobrist == rBoard.ZbHash) {
				eval = zobrist.transTable[hashCode].value;
				countTrans++;
			}
			else {
				countEval++;
				eval = rBoard.evaluating(useFlyEval);
				if (dep >= zobrist.transTable[hashCode].depth) {
					if (zobrist.transTable[hashCode].depth != 0)
						countNewTrans++;
					zobrist.transTable[hashCode].depth = dep;
					zobrist.transTable[hashCode].value = eval;
					zobrist.transTable[hashCode].zobrist = rBoard.ZbHash;
				}
			}
		}
		else {
			eval = rBoard.evaluating(useFlyEval);
			countEval++;
		}
		//rBoard.draw();
		//std::cout << "eval:" << eval << std::endl;
		return nplayer == FIRST_PLAYER ? eval : -eval;
	}

	MoveMaker moves(rBoard,nplayer);
	Board board=rBoard;
	ChessState oppo = getOppoPlayer(nplayer);
	if (moves.empty()) {
		return -AlphaBeta(-beta, -alpha, 0, board, oppo);
	}
	int ret;
	JiuMove bestMove;
	for (JiuMove m = moves.getFirstMove(); moves.isEndding() != true; m = moves.getNextMove()) {
		board = rBoard;
		ret=board.doMoves(m);
		if (ret < 0)
			continue;
		ret = repeatedDetection(board, depth);
		if (ret < 0)
			continue;
		historyHash[depth] = board.ZbHash;
		//board.drawWithMove(m);
		int value = -AlphaBeta(-beta, -alpha, depth - 1, board,oppo);
		clock_t mem_end = clock();
		if (mem_end - mem_start > maxTime)
			return alpha;
		if (value >= beta) {
			addHistory(m, depWeight);
			return beta;
		}
		if (value > alpha) {
			bestMove = m;
			alpha = value;
		}
	}
	addHistory(bestMove, depWeight);
	return alpha;
}

const int Minimax::repeatedDetection(Board & board,int depth)
{
	//探测历史记录
	int i = 0;
	for (auto item = gameHistoryHash.rbegin(); item != gameHistoryHash.rend(); item++) {
		if (i >= REPEATED_DRAW_ROUNDS)
			break;	
		if (*item == board.ZbHash)
			return -1;
	}
	//探测已走步
	for (i = depth + 1; i <= dep; i++) {
		if (historyHash[i] == board.ZbHash)
			return -1;
	}
	return 0;
}

JiuMove Minimax::AlphaBetaFirst(int alpha, int beta, int depth, Board & rBoard)
{
	MoveMaker moves(rBoard, player);
	int ret;
	Board board = rBoard;
	ChessState oppo = getOppoPlayer(player);

	//优先评估之前选出的最佳走步
	if (dep > 1) {
		ret = board.doMoves(bestMoveFirst);
		historyHash[depth] = board.ZbHash;
		if (ret == 0) {
			int value = -AlphaBeta(-beta, -alpha, depth - 1, board, oppo);
			clock_t mem_end = clock();
			if (mem_end - mem_start > maxTime)
				return bestMoveFirst;
			if (value > alpha) {
				alpha = value;
				bestEval = alpha;
			}
		}
	}

	for (JiuMove m = moves.getFirstMove(); moves.isEndding() != true; m = moves.getNextMove()) {
		board = rBoard;
		ret = board.doMoves(m);
		if (ret < 0)
			continue;
		ret = repeatedDetection(board, depth);
		if (ret < 0)
			continue;
		historyHash[depth] = board.ZbHash;
		//board.drawWithMove(m);
		int value = -AlphaBeta(-beta, -alpha, depth - 1, board,oppo);
		clock_t mem_end = clock();
		if (mem_end - mem_start > maxTime)
			return bestMoveFirst;
		if (value > alpha) {
			alpha = value;
			bestEval = alpha;
			bestMoveFirst = m;
		}
	}
	addHistory(bestMoveFirst, depWeight);
	return bestMoveFirst;
}

JiuMove Minimax::ABsearch()
{
	mem_start = clock();
	JiuMove ret,bestMove;

	for (int i = 1; i <= maxDepth; i++) {
		dep = i;
		depWeight = i*i;
		resetCounts();
		zobrist.resetTransTable();

		if(output)
			std::cout << "开始搜索迭代加深搜索，当前第" << i << "层\t";
		ret=AlphaBetaFirst(-INT_MAX, INT_MAX, i, rootBoard);
		clock_t mem_end = clock();
		if (output)
			std::cout << "搜索结束，评价" << countEval << "次,"
				<< "评价值为" << bestEval << ","
		//		<<"置换表命中"<<countTrans<<"次,"
		//		<< "置换表替换" << countNewTrans << "次,"
				<<"共耗费"<< (double)(mem_end - mem_start) / CLOCKS_PER_SEC <<"s,"
				<< "该层最佳走步:" << ret
				<< "\n";
		if(ret.moveState!=ILLEGAL_PLAYER)
			bestMove = ret;
		if (mem_end - mem_start > maxTime) {
			break;
		}

	}
	std::cout << "搜索深度为" << dep;
	std::cout << ",局面评价值为" << ((player == FIRST_PLAYER) ? bestEval : -bestEval) << "\n";
	declineHistory();
	return bestMove;
}

Minimax::~Minimax()
{
}






LayoutMinimax::LayoutMinimax(Board board, ChessState player, bool useHash, int evalRule):
	rootBoard(board),
	player(player),
	useHash(useHash),
	evalRule(evalRule)
{
	if (evalRule == 0) {
		rulesWeight[0] = 1;
		rulesWeight[1] = 0;
	}
	else if (evalRule == 1) {
		rulesWeight[0] = 0;
		rulesWeight[1] = 1;
	}
	else if (evalRule == 2) {
		rulesWeight[0] = 0.5;
		rulesWeight[1] = 0.5;
	}
}

LayoutMinimax::~LayoutMinimax()
{
}

int LayoutMinimax::AlphaBeta(int alpha, int beta, int depth, Board & rBoard, ChessState nplayer)
{
	if (depth == 0) {
		int eval = INT_MAX;
		if (useHash) {
			int hashCode = rBoard.ZbHash >> (64 - ZbLayoutSize);
			if (zobrist.layoutTransTable[hashCode].zobrist == rBoard.ZbHash) {
				eval = zobrist.layoutTransTable[hashCode].value;
				countTrans++;
			}
			else {
				countEval++;
				eval = rBoard.layoutEval();

				if (dep >= zobrist.layoutTransTable[hashCode].depth) {
					if (zobrist.layoutTransTable[hashCode].depth != 0)
						countNewTrans++;
					zobrist.layoutTransTable[hashCode].depth = dep;
					zobrist.layoutTransTable[hashCode].value = eval;
					zobrist.layoutTransTable[hashCode].zobrist = rBoard.ZbHash;
				}
			}
		}
		else {
			countEval++;
			if (evalRule == 3) {
				double blk = rBoard.countBlankPieces();
				rulesWeight[1] = blk / BOARD_PIECE_LIMIT;
				rulesWeight[0] = 1 - rulesWeight[1];
			}
			eval = rBoard.layoutEval(rulesWeight[0], rulesWeight[1]);
		}
		/*rBoard.draw();
		std::cout << "eval:" << eval << std::endl;;
		getchar();*/

		return nplayer == FIRST_PLAYER ? eval : -eval;
	}

	LayoutPositionMaker lpm(rBoard);
	int ret;
	Board board = rBoard;
	ChessState oppo = getOppoPlayer(nplayer);
	JiuMove m;
	m.player = nplayer;
	m.moveState = ADD_PIECE;
	PiecePosition p, bestp;
	if (lpm.size() == 0)
		return -AlphaBeta(-beta, -alpha, depth - 1, board, oppo);
	else
		for (int i = 0; i < lpm.size(); i++) {
			board = rBoard;
			p = lpm[i];
			m.initPos(p.pos[0], p.pos[1], p.pos[0], p.pos[1]);
			ret = board.doMoves(m);
			if (ret < 0)
				continue;
			int value = -AlphaBeta(-beta, -alpha, depth - 1, board, oppo);
			clock_t mem_end = clock();
			if (mem_end - mem_start > maxTime) {
				return alpha;
			}
			if (value >= beta) {
				historyTable[bestp.pos[0]][bestp.pos[1]] += depWeight;
				return beta;
			}
			if (value > alpha) {
				alpha = value;
				bestp = p;
			}

		}
	historyTable[bestp.pos[0]][bestp.pos[1]] += depWeight;
	return alpha;
}

JiuMove LayoutMinimax::AlphaBetaFirst(int alpha, int beta, int depth, Board & rBoard)
{
	LayoutPositionMaker lpm(rBoard);
	int ret;
	Board board = rBoard;
	ChessState oppo = getOppoPlayer(player);
	JiuMove m;
	m.player = player;
	m.moveState = ADD_PIECE;
	PiecePosition p;

	if (dep > 1) {
		m.initPos(bestPosFirst.pos[0], bestPosFirst.pos[1], bestPosFirst.pos[0], bestPosFirst.pos[1]);
		ret = board.doMoves(m);
		int value = -AlphaBeta(-beta, -alpha, depth - 1, board, oppo);
		clock_t mem_end = clock();
		if (mem_end - mem_start > maxTime)
			return m;
		if (value > alpha) {
			alpha = value;
			bestEval = alpha;
		}
	}
	for (int i = 0; i < lpm.size(); i++) {
		board = rBoard;
		p = lpm[i];
		m.initPos(p.pos[0], p.pos[1], p.pos[0], p.pos[1]);
		ret = board.doMoves(m);
		if (ret < 0)
			continue;
		int value = -AlphaBeta(-beta, -alpha, depth - 1, board, oppo);
		clock_t mem_end = clock();
		if (mem_end - mem_start > maxTime) {
			m.initPos(bestPosFirst.pos[0], bestPosFirst.pos[1], bestPosFirst.pos[0], bestPosFirst.pos[1]);
			return m;
		}
		if (value > alpha) {
			alpha = value;
			bestEval = alpha;
			bestPosFirst = p;
		}
	
	}
	historyTable[bestPosFirst.pos[0]][bestPosFirst.pos[1]] += depWeight;
	m.initPos(bestPosFirst.pos[0], bestPosFirst.pos[1], bestPosFirst.pos[0], bestPosFirst.pos[1]);
	return m;
}

JiuMove LayoutMinimax::ABsearch()
{
	mem_start = clock();
	JiuMove ret, bestMove;

	for (int i = 1; i <= maxDepth; i++) {
		dep = i;
		depWeight = i * i;
		resetCounts();
		zobrist.resetLayoutTransTable();
		if (output)
			std::cout << "开始搜索迭代加深搜索，当前第" << i << "层\t";
		ret = AlphaBetaFirst(-INT_MAX, INT_MAX, i, rootBoard);
		clock_t mem_end = clock();
		if (output)
			std::cout << "搜索结束，评价" << countEval << "次,"
		//		<< "置换表命中" << countTrans << "次,"
		//		<< "置换表替换" << countNewTrans << "次,"
				<< "共耗费" << (double)(mem_end - mem_start) / CLOCKS_PER_SEC << "s,"
				<< "该层最佳走步:" << ret
				<<"\n";
		if(ret.moveState!=ILLEGAL_PLAYER)
			bestMove = ret;
		if (mem_end - mem_start > maxTime) {
			break;
		}

	}
	std::cout << "搜索深度为" << dep;
	std::cout << ",局面评价值为" << ((player == FIRST_PLAYER) ? bestEval : -bestEval) << "\n";
	declineHistory();
	bestMove.player = player;
	return bestMove;
}
