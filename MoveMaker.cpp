#include "pch.h"
#include "MoveMaker.h"
#include "ManualReader.h"



const MoveMaker::Direction MoveMaker::DIRECTIONS[4] = {
MoveMaker::UP,
MoveMaker::LEFT,
MoveMaker::DOWN,
MoveMaker::RIGHT
};

int MoveMaker::createOppoPieces()
{
	if (hasCreOP)
		return 0;
	hasCreOP = true;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.getPieceState(i, j) == oppo) {
				oppoPieces.push_back(PiecePosition({i,j}));
			}
		}
	}
	std::sort(oppoPieces.begin(), oppoPieces.end());
	return 1;
}

JiuMove MoveMaker::Iter2JiuMove()
{
	HalfMove &hm = HalfMoves[moveIter.index];
	JiuMove jm;
	jm.xStartPosition = hm.x;
	jm.yStartPosition = hm.y;

	jm.player = player;

	if (hm.state == STEP) {
		jm.moveState = STEP_PIECE;
		jm.xEndPosition = hm.x + trans[hm.dirList[0]][0];
		jm.yEndPosition = hm.y + trans[hm.dirList[0]][1];
	}
	else if(hm.state == JUMP) {
		jm.moveState = JUMP_PIECE;
		int x = hm.x;
		int y = hm.y;
		for (auto dir : hm.dirList) {
			//将跳过的对方子加入cap列表
			jm.addCapPos(x + trans[dir][0], y+trans[dir][1]);
			//改变当前位置
			x = x + jTrans[dir][0];
			y = y + jTrans[dir][1];
			//将跳跃路径加入jump列表
			jm.addJumpPos(x, y);
		}
		jm.xEndPosition = x;
		jm.yEndPosition = y;
	}
	else {
		jm.moveState = FLY_PIECE;
		jm.xEndPosition = hm.xe;
		jm.yEndPosition = hm.ye;
	}
	for (int i = 0; i < hm.captureMax; i++) {
		int oppoInd = moveIter.posInd[i];
		PiecePosition& piece = oppoPieces[oppoInd];
		jm.addCapPos(piece.pos[0],piece.pos[1]);
	}

	return jm;
}

int MoveMaker::iterMoveDirec(Direction direc,int &x,int &y)
{
	if (iter.state == STEP) {
		int ex = x + trans[(int)direc][0];
		int ey = y + trans[(int)direc][1];
		if (board.isState(ex, ey, NON_PLAYER)) {
			x = ex;
			y = ey;
			iter.dirList.push_back(direc);
			return 1;
		}
	}
	else {
		int jx = x + trans[(int)direc][0];
		int jy = y + trans[(int)direc][1];
		if (board.isState(jx, jy, oppo)) {
			int ex = x + jTrans[(int)direc][0];
			int ey = y + jTrans[(int)direc][1];
			//判定成功后，状态向该方向转移，并向栈中加入UNDO操作
			// ↓ → 
			// 成功： ↓ → ---》 ↑←↓→ UNDO →
			// 失败： ↓ → ---》 →
			if (board.isState(ex, ey, NON_PLAYER)) {
				board.setPieceState(jx,jy,NON_PLAYER);//跳跃中删去子，以防止循环跳跃,在UNDO时要对应补回该棋子
				x = ex;
				y = ey;
				iter.dirList.push_back(direc);//iter栈加入当前方向
				direcStack.push_back(UNDO);//UNDO入方向栈
				for (auto d : MoveMaker::DIRECTIONS) {
					direcStack.push_back(d);
				}
				return 1;
			}
		}
	}
	return 0;
}

int MoveMaker::iterUndo(int &x,int &y)
{
	if (iter.dirList.empty()) {
		return -1;
	}
	if (iter.state == STEP) {
		Direction direc = iter.dirList.back();
		x = x - trans[(int)direc][0];
		y = y - trans[(int)direc][1];
		iter.dirList.pop_back();
	}
	else {
		auto direc = iter.dirList.back();
		int jx = x - trans[(int)direc][0];
		int jy = y - trans[(int)direc][1];
		board.setPieceState(jx, jy, oppo);//撤销跳跃时，补回该子
		x = x - jTrans[(int)direc][0];
		y = y - jTrans[(int)direc][1];
		iter.dirList.pop_back();
	}
	return 0;
}

int MoveMaker::PieceHalfMoves(int xPiece,int yPiece)
{
	iter.x = xPiece;
	iter.y = yPiece;
	int x = xPiece, y = yPiece;
	if (player != board.getPieceState(x, y))
		return -1;
	iter.dirList.clear();
	int ret = false;
	iter.state = STEP;
	for (auto d : MoveMaker::DIRECTIONS) {
		ret=iterMoveDirec(d, x, y);
		if (ret==1) {
			iter.captureMax = conBoard.preMove(xPiece, yPiece, x, y);
			HalfMoves.push_back(iter);
			iterUndo(x, y);
		}
	}

	iter.state = JUMP;
	//初始化：
	//方向栈：↑←↓→
	//iter栈：NULL
	for (auto d : MoveMaker::DIRECTIONS) {
		direcStack.push_back(d);
	}
	Direction d;
	//第一步
	//方向栈：（↑←↓→UNDO）←↓→
	//iter栈：（↑）NULL
	while (!direcStack.empty()) {
		d = direcStack.back();
		direcStack.pop_back();
		if (d == UNDO) {
			iterUndo(x, y);
		}
		else {
			ret=iterMoveDirec(d, x, y);
			if (ret == 1) {
				iter.captureMax = conBoard.preMove(xPiece, yPiece, x, y);
				HalfMoves.push_back(iter);
			}
		}
	}
	return 0;
}

int MoveMaker::PieceHalfFlyMoves(int xPiece, int yPiece)
{
	if (player != board.getPieceState(xPiece, yPiece))
		return -1;
	iter.x = xPiece;
	iter.y = yPiece;
	iter.state = FLY;
	iter.dirList.clear();
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (conBoard.isState(i, j, NON_PLAYER)) {
				iter.xe = i;
				iter.ye = j;
				iter.captureMax = conBoard.preMove(xPiece, yPiece, i, j);
				HalfMoves.push_back(iter);
			}
		}
	}
	return 0;
}


int MoveMaker::makeHalfMoves()
{
	if (isMaked)
		return -1;
	isMaked = true;
	if (conBoard.boardState == Board::ENDDING)
		return 0;

	isFly=conBoard.isFlyPlayer(player);

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			PieceHalfMoves(i, j);
			if (isFly) {
				PieceHalfFlyMoves(i, j);
			}
		}
	}

	std::sort(HalfMoves.begin(), HalfMoves.end());
	return 0;
}

JiuMove MoveMaker::getFirstMove()
{
	if (HalfMoves.empty()) {
		std::cout << "没有可行的下法\n";
		return JiuMove::errorJiuMove();
	}
	if (hasCreOP == false) {
		createOppoPieces();
	}
	moveIter.index = 0;
	for (int i = 0; i < HalfMoves[moveIter.index].captureMax; i++) {
		moveIter.posInd[i] = i;
	}
	return Iter2JiuMove();
}

JiuMove MoveMaker::getNextMove()
{
	int cap = HalfMoves[moveIter.index].captureMax;
	bool flag = true;
	//如果可提子为4，迭代步为12 13 14 23 24 34
	for (int i = 0; i<	cap; i++) {
		if (flag) {
			moveIter.posInd[cap-1-i]++;
		}
		if (moveIter.posInd[cap-1-i] < (oppoPieces.size()-i)) {
			flag = false;
			break;
		}
	}
	if (flag==false) {
		for (int i = 0; i < cap; i++) {
			if (moveIter.posInd[i] > (oppoPieces.size() - cap+ i)) {
				moveIter.posInd[i] = moveIter.posInd[i - 1] + 1;
			}
		}
	}
	else {
		moveIter.index++;
		if (moveIter.index >= HalfMoves.size()) {
			JiuMove err;
			err.moveState = ERROR_PIECE;
			return err;
		}
		for (int i = 0; i < HalfMoves[moveIter.index].captureMax; i++) {
			moveIter.posInd[i] = i;
		}
	}
	return Iter2JiuMove();
}

bool MoveMaker::isEndding()
{
	return moveIter.index >=HalfMoves.size();
}

int MoveMaker::writeHalfMoves()
{
	char pos[3];
	pos[2] = '\0';
	for (int i = 0; i < HalfMoves.size(); i++) {
		HalfMove &m = HalfMoves[i];
		int x = m.x;
		int y = m.y;

		std::cout << i << '\t'<<m.captureMax<<"\t" <<m.getCaptureJump()<<" :\t";

		ManualReader::Num2Str(x, y, player, pos);

		std::cout << pos;

		for (auto dir : m.dirList) {
			if (m.state == STEP) {
				x = x + trans[(int)dir][0];
				y = y + trans[(int)dir][1];
			}
			else {
				x = x + jTrans[(int)dir][0];
				y = y + jTrans[(int)dir][1];
			}
			ManualReader::Num2Str(x, y, player, pos);
			std::cout << '-' << pos;
		}
		std::cout << std::endl;
	}
	return 0;
}

MoveMaker::MoveMaker(Board board, ChessState player):
	board(board),
	conBoard(board),
	player(player),
	oppo(getOppoPlayer(player))
{
	makeHalfMoves();
}


MoveMaker::~MoveMaker()
{
}

PiecePosition & LayoutPositionMaker::operator[](int i)
{
	return NonPieces[i];
}

int LayoutPositionMaker::makeNonPieces()
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board.isState(i, j, NON_PLAYER))
				NonPieces.push_back({ i,j });
		}
	}
	std::sort(NonPieces.begin(), NonPieces.end());
	return 0;
}
