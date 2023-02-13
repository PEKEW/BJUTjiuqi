#include "pch.h"
#include "Game.h"
#include "HumanPlayer.h"
#include "GongJiuPlayer.h"
#include "MoveMaker.h"
#include "Minimax.h"
#include <fstream>


int Game::handleFlags()
{
	if (repeatedDrawFlag) {
		std::cout << "\n-----局面重复判和-----\n" << std::endl;
		return -1;
	}
	if (peaceDrawFlag) {
		std::cout << "\n----长期不吃子判和-----\n" << std::endl;
		return -1;
	}
	return 0;
}





int Game::GameStage(Board::BoardState stage)
{
	JiuMove m;
	int peaceTurn = 0;
	int peacePieces = board.countBlankPieces();
	int countStageTrun = 0;//当前状态进行轮数
	while (board.boardState == stage) {
		std::cout << "轮到玩家" << (currentPlayer + 1) << "行动\n";
		m = p[currentPlayer]->getMove(board, lastMove);
		if (undoFlag == true) {
			undoFlag = false;
			if (countStageTrun < 2) {
				std::cout << "当前状态不足两步，无法悔棋\n";
			}
			else {
				countStageTrun -= 2;
				size_t undoSize = replayBoards.size() - 2;
				replayMoves.resize(replayMoves.size()-2);
				lastMove = replayMoves.back();
				replayBoards.resize(undoSize);
				replayHashs.resize(undoSize);
				board = replayBoards.back();
			}
			continue;
		}

		if (changeBoardFlag) {
			changeBoardFlag = false;
			std::cout << "------------------\n调整模式，请输入棋子坐标,棋子将由空，*,o顺序调整\n";
			std::string s;
			std::cin >> s;
			int x=0, y=0;
			if(s.size()>=2)
				ManualReader::Str2Position(s, x, y);
			ChessState tmp = board.getPieceState(x, y);
			if (s.size() > 2) {
				if (s[2] >= '0' && s[2] <= '2') {
					tmp = ChessState(s[2] - '0');
				}
			}else if (tmp == NON_PLAYER)
				tmp = FIRST_PLAYER;
			else if (tmp == FIRST_PLAYER)
				tmp = SECOND_PLAYER;
			else if (tmp == SECOND_PLAYER)
				tmp = NON_PLAYER;
			board.setPieceState(x, y, tmp);
			replayBoards.pop_back();
			replayHashs.pop_back();
			replayBoards.push_back(board);
			replayHashs.push_back(board.ZbHash);
			std::cout << "调整完毕\n------------------\n";
			continue;
		}
		int ret = board.doMoves(m);
		if (ret != 0) {
			std::cout << "输入错误，请玩家" << (currentPlayer + 1) << "重新行动\n";
			board = replayBoards.back();
			continue;
		}
		countStageTrun++;

		if (replayRulesFlag) {
			if (repeatedDetection() == 1) {
				repeatedDrawFlag = true;
				return 0;
			}
		}
		replayMoves.push_back(m);
		replayBoards.push_back(board);
		replayHashs.push_back(board.ZbHash);

		if (writeReplayFlag) {
			writeReplayToFile();
		}

		//指定回合内不吃子，判平局
		if (peaceRulesFlag&&board.countBlankPieces() == peacePieces) {
			peaceTurn++;
			if (peaceTurn > PEACE_ROUNDS) {
				peaceDrawFlag = true;
				return 0;
			}
		}
		else {
			peaceTurn = 0;
			peacePieces = board.countBlankPieces();
		}
		currentPlayer = 1 - currentPlayer;
		lastMove = m;
	}
	return 0;
}

int Game::addPlayPtr(PlayerPtr p1, PlayerPtr p2)
{
	p[0] = std::move(p1);
	p[0]->player = FIRST_PLAYER;
	std::cout << "p1由" << p[0]->name << "操控" << std::endl;
	p[1] = std::move(p2);
	p[1]->player = SECOND_PLAYER;
	std::cout << "p2由" << p[1]->name << "操控" << std::endl;
	return 0;
}

int Game::addHumanPlayer()
{
	return addPlayer<HumanPlayer>();
}

int Game::addGongJiuPlayer()
{
	return addPlayer<GongJiuPlayer>();
}




int Game::startGame()
{
	replayBoards.push_back(board);
	replayHashs.push_back(board.ZbHash);
	replayMoves.push_back(JiuMove::errorJiuMove());
	std::cout << "\n------游戏开始------\n\n";
	GameStage(Board::LAYOUT);
	std::cout << "\n---布局结束,移子阶段开始---\n" << std::endl;

	currentPlayer = 1;
	board.removeCenterPieces();

	replayBoards.push_back(board);
	replayHashs.push_back(board.ZbHash);

	GameStage(Board::MOVE);
	if (handleFlags()<0)
		return 0;

	std::cout << "\n-----进入飞子阶段-----\n" << std::endl;
	GameStage(Board::FLY);
	if (handleFlags()<0)
		return 0;
	std::cout << "\n\n------游戏结束------\n"
		<< "最终局面为\n";
	board.drawWithMove(lastMove);
	int winner = board.gameEndding();
	std::cout << "获胜方是p" << winner << ":" << p[winner - 1]->name<<"\n";
	return winner;
}


int Game::takeOverReplay(JiuMove last)
{
	//addHumanPlayer();
	addGongJiuPlayer();
	addGongJiuPlayer();
	
	std::cout << "\n------接管录像------\n\n";
	if (board.boardState == Board::LAYOUT) {
		GameStage(Board::LAYOUT);
		std::cout << "布局结束,移子阶段开始" << std::endl;
	}
	if (board.countBlankPieces() < 2) {
		currentPlayer = 1;
		board.removeCenterPieces();
	}
	if (board.boardState == Board::MOVE) {
		GameStage(Board::MOVE);
		std::cout << "进入飞子阶段" << std::endl;
	}
	GameStage(Board::FLY);
	return 0;
}

int Game::takeOverCanJu(bool useAI1,bool useAI2,ChessState cp)
{
	if(useAI1)
		addGongJiuPlayer();
	else
		addHumanPlayer();
	if (useAI2)
		addGongJiuPlayer();
	else
		addHumanPlayer();
	//addGongJiuPlayer();
	replayBoards.push_back(board);
	replayHashs.push_back(board.ZbHash);
	currentPlayer = cp-1;
	std::cout << "\n------接管录像------\n\n";
	if (board.boardState == Board::LAYOUT) {
		GameStage(Board::LAYOUT);
		std::cout << "布局结束,移子阶段开始" << std::endl;
	}
	if (board.countBlankPieces() < 2) {
		currentPlayer=1 ;
		board.removeCenterPieces();
	}
	if (board.boardState == Board::MOVE) {
		GameStage(Board::MOVE);
		std::cout << "进入飞子阶段" << std::endl;
	}
	GameStage(Board::FLY);
	return 0;
}

int Game::readMode(std::string path)
{
	replay.readFile(path);

	board.draw();
	replayBoards.push_back(board);
	char c;
	char n;
	int skip = 0;
	int i = 0;
	bool cmd = false;
	
	int halfCount=0;
	int halfTimes=0;
	while (true) {
		c = getchar();
		//c = '\n';
		n = c;
		while (n != '\n')
			n = getchar();

		if (i>197&&(c == 'q'||c=='\n')) {

			MoveMaker m(board, replay[i].player);
			m.makeHalfMoves();
			halfCount += m.HalfCounts();
			halfTimes++;
			m.writeHalfMoves();
			//cmd = true;
		}
		else if (c=='w') {
			Minimax minimax(board, replay[i].player, replayHashs);
			JiuMove ret=minimax.ABsearch();
			std::cout << "当前最佳走步为: " << ret << std::endl;

		}
		else if (c=='t') {
			return takeOverReplay(replay[i - 1]);
		}
		else if (c == 'a') {
			skip += 5;
		}
		else if (c == 's') {
			skip += 20;
		}
		else if (c == 'd') {
			skip += 100;
		}
		else if (c == 'f') {
			if (i < 196)
				skip += 196 - i;
		}
		if (cmd) {
			cmd = false;
			continue;
		}
		if (skip == 0)
			skip ++;
		while (skip>0 && i<replay.size) {
			skip--;
			board.doMoves(replay[i]);
			replayBoards.push_back(board);
			replayHashs.push_back(board.ZbHash);
			i++;

		}
		if(i>0)
			board.drawWithMove(replay[i-1]);
		if (i == replay.size)
			break;
	}
	std::cout << "counts:" << halfCount << std::endl;
	std::cout << "Times:" << halfTimes << std::endl;
	std::cout << "averange:" << (double)halfCount / halfTimes;
	return 0;
}

int Game::setReplayFilePath(std::string path)
{
	filePath = path;
	return 0;
}

int Game::writeReplayToFile()
{
	std::ofstream fout(filePath);
	fout << p[0]->name << " vs " << p[1]->name<<'\n';
	bool changeStage = false;
	int layoutEnd = 0;
	for (int i = 0; i < replayMoves.size(); i++) {
		if (replayMoves[i].moveState == ADD_PIECE)
			fout << "0." << replayMoves[i]<<"\n";
		else {
			if (!changeStage) {
				changeStage = true;
				fout << "-----布局结束-----\n";
				layoutEnd = i-1;
			}
			fout<<(i-layoutEnd)<<"."<< replayMoves[i] << "\n";
		}
	}
	fout.close();
	return 0;
}



int Game::repeatedDetection()
{
	if (!replayRulesFlag)
		return 0;
	
	for (int i = replayBoards.size() - 1; i >= 0&&i> replayBoards.size() - 1- REPEATED_DRAW_ROUNDS; i--) {
		if (replayHashs[i] == board.ZbHash)
			return 1;
	}
	return 0;
}

Game::~Game()
{
}
