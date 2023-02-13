#include "pch.h"
#include "ManualReader.h"
#include <string>
#include <fstream>

ManualReader::ManualReader()
{
}


ChessState ManualReader::Str2Position(std::string str,int &x,int& y) {
	ChessState player=NON_PLAYER;
	if (str[0] >= 'a'&&str[0] <= 'n') {
		x = str[0] - 'a';
		player = FIRST_PLAYER;
	}
	else if (str[0] >= 'A'&&str[0] <= 'N') {
		x = str[0] - 'A';
		player = SECOND_PLAYER;
	}
	else {
		std::cout << "输入坐标有误\n";
		x = -1;
		y = -1;
		return ILLEGAL_PLAYER;
	}

	if (str[1] >= ('0'+ROW_OUTPUT_ADJUST)&&str[1] <= '9')
		y = str[1] - ('0' + ROW_OUTPUT_ADJUST);
	else if(str[1] >= 'A'&&str[1] <= ('D'+ROW_OUTPUT_ADJUST))
		y = 10 + str[1] - 'A'-ROW_OUTPUT_ADJUST;
	else if(str[1] >= 'a'&&str[1] <= ('d' + ROW_OUTPUT_ADJUST))
		y = 10 + str[1] - 'a'-ROW_OUTPUT_ADJUST;
	else {
		std::cout << "输入坐标有误\n";
		x = -1;
		y = -1;
		return ILLEGAL_PLAYER;
	}

	return player;
}

int ManualReader::Num2Str(int x, int y, ChessState player, char ret[2])
{
	if (player == FIRST_PLAYER)
		ret[0] = ('a' + x);
	else
		ret[0] = ('A' + x);

	if (y < (10-ROW_OUTPUT_ADJUST))
		ret[1] = ('0') + y+ ROW_OUTPUT_ADJUST;
	else 
		ret[1] = ('A' + y - 10+ ROW_OUTPUT_ADJUST);
	return 0;
}


JiuMove ManualReader::Str2JiuMove(std::string str,ChessState player,MoveState moveState)
{
	size_t pos = str.find(','); 
	JiuMove move;
	//没有','代表棋谱中没有提子，那么就只能有以下两种情况
	//xx;开局两步提走中心子
	//xx-xx;走一步
	if (moveState == ADD_PIECE) {
		int x=-1, y=-1;
		move.player = player;
		if (Str2Position(str, x, y) == ILLEGAL_PLAYER)
			return JiuMove::errorJiuMove();
		if(x<0||x>=BOARD_SIZE)
			return JiuMove::errorJiuMove();
		if (y < 0 || y >= BOARD_SIZE)
			return JiuMove::errorJiuMove();
		move.initPos(x, y, x, y);
		move.moveState = moveState;
		return move;
	}



	if (pos == std::string::npos) {
		size_t tpos = str.find('-');

		//没有'-'，代表开局两步提掉中心子，在move中标记起点终点为同一个
		if (tpos == std::string::npos) {
			int x=-1, y=-1;
			move.player=Str2Position(str, x, y);
			if (move.player == ILLEGAL_PLAYER)
				return JiuMove::errorJiuMove();
			move.initPos(x, y, x, y);
			move.moveState = DROP_PIECE;
		}
		//有'-'，代表无提子常规移动
		else {
			int xS = -1, yS = -1;
			move.player=Str2Position(str, xS, yS);
			if (move.player == ILLEGAL_PLAYER)
				return JiuMove::errorJiuMove();
			std::string sub = str.substr(tpos + 1);
			int xE = -1, yE = -1;
			move.player=Str2Position(sub, xE, yE);
			if (move.player == ILLEGAL_PLAYER)
				return JiuMove::errorJiuMove();
			move.initPos(xS, yS, xE, yE);
		}
	}
	//有提子情况，分为有跳吃和无跳吃两种
	//先分为第一个逗号前后，把前半段录入跳跃部分
	else {
		//分为两部分，mov行动，cap提子
		std::string mov = str.substr(0, pos);
		std::string cap = str.substr(pos + 1);
		//先处理mov，找到移动开始的位置
		int xS = -1, yS = -1;
		move.player=Str2Position(str, xS, yS);
		if (move.player == ILLEGAL_PLAYER)
			return JiuMove::errorJiuMove();
		//然后逐步处理，添加到跳跃点，直到最后，再将最后的位置添加至结束

		size_t movpos = mov.find('-');
		while (movpos != std::string::npos) {
			int xJ = -1, yJ = -1;
			if(Str2Position(mov, xJ, yJ)==ILLEGAL_PLAYER)
				return JiuMove::errorJiuMove();
			move.addJumpPos(xJ, yJ);

			mov = mov.substr(movpos + 1);
			movpos = mov.find('-');
		}

		int xE = -1 , yE = -1;
		if(Str2Position(mov, xE, yE)==ILLEGAL_PLAYER)
			return JiuMove::errorJiuMove();
		move.addJumpPos(xE, yE);
		move.initPos(xS, yS, xE, yE);


		//接下来处理提子部分，按顺序添加到提子就行
		size_t cappos = cap.find(',');
		while (cappos != std::string::npos) {
			int xC = -1, yC = -1;
			if(Str2Position(cap, xC, yC)==ILLEGAL_PLAYER)
				return JiuMove::errorJiuMove();
			move.addCapPos(xC, yC);

			cap = cap.substr(cappos + 1);
			cappos = cap.find(',');
		}
		int xC = -1, yC = -1;
		Str2Position(cap, xC, yC);
		move.addCapPos(xC, yC);
	}

	if (player != NON_PLAYER)
		move.player = player;

	return move;
}



ManualReader::~ManualReader()
{
}

ReplayReader::ReplayReader()
{
}

ReplayReader::ReplayReader(std::string path)
{
	readFile(path);
}

int ReplayReader::readFile(std::string path)
{
	std::ifstream replayFile(path);
	std::string str;

	//读入第一行，布局行
	replayFile >> str;
	size_t pos = str.find(',');
	std::string order, sub;
	order = str.substr(0, pos);
	sub = str.substr(pos + 1);

	//检查当前棋谱编号，防止漏招
	int countOrder = 0;
	if (std::stoi(order) != countOrder)
		std::cout << "没有布局阶段棋谱" << std::endl;

	//解析布局阶段
	int x = -1, y = -1;
	JiuMove move;
	pos = sub.find(',');
	while (pos != std::string::npos) {
		move.player=ManualReader::Str2Position(sub, x, y);
		move.initPos(x, y, x, y);
		move.moveState = ADD_PIECE;
		layoutMoves.push_back(move);
		sub = sub.substr(pos + 1);
		pos = sub.find(',');
	}
	move.player=ManualReader::Str2Position(sub, x, y);
	move.initPos(x, y, x, y);
	layoutMoves.push_back(move);

	//解析移子阶段
	while (replayFile >> str) {
		pos = str.find(',');

		order = str.substr(0, pos);
		countOrder++;
		if (std::stoi(order) != countOrder)
			std::cout << "棋谱编号存在错误" << std::endl;

		sub = str.substr(pos + 1);
		Moves.push_back(ManualReader::Str2JiuMove(sub));
	}
	countLayout = layoutMoves.size();
	countMoves = Moves.size();
	size = countLayout + countMoves;
	return 0;
}

JiuMove ReplayReader::operator[](int index)
{
	if (index >= size)
		throw "越界";
	if (index < countLayout)
		return layoutMoves[index];
	return Moves[index - countLayout];
}


ReplayReader::~ReplayReader()
{
}

std::ostream & operator<<(std::ostream & out, JiuMove & m)
{
	char pos[3];
	pos[2] = '\0';

	ChessState player = m.player;

	ManualReader::Num2Str(m.xStartPosition, m.yStartPosition, player, pos);

	out << pos;
	if (m.moveState == STEP_PIECE||m.moveState==FLY_PIECE) {
		ManualReader::Num2Str(m.xEndPosition, m.yEndPosition, player, pos);
		out << '-' << pos ;
	}
	else if (m.moveState == JUMP_PIECE) {
		for (auto p : m.jumpPositions) {
			ManualReader::Num2Str(p[0], p[1],player,pos);
			out << '-' << pos;
		}
	}

	for (int i = m.jumpCaptureCount; i < m.capturePositions.size(); i++) {
		ManualReader::Num2Str(m.capturePositions[i][0], m.capturePositions[i][1], player, pos);
		out << ',' << pos;
	}
	out << ';';
	return out;
}