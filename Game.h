#pragma once
#include "Board.h"
#include "Player.h"
#include "ManualReader.h"
#include <memory>


typedef std::unique_ptr<Player> PlayerPtr;
class Game
{
	Board board;
	ReplayReader replay;//读谱器
	std::array<PlayerPtr,2> p = { nullptr,nullptr };//两个玩家指针，p[0]是先手玩家

	JiuMove lastMove=JiuMove::errorJiuMove();//保存上一步

	bool writeReplayFlag = false;//是否输出棋谱
	std::string filePath;//输出棋谱的文件位置

	bool replayRulesFlag = true;//是否启用重复判负规则
	bool peaceRulesFlag = true;//是否启用长时间不吃子判平规则

	bool changeBoardFlag = false;//修改局面函数标志，玩家用
	bool undoFlag = false;//撤销上一步，玩家用

	bool repeatedDrawFlag = false;//重复判定平局，内部变量
	bool peaceDrawFlag = false;//无吃子判定平局，内部变量

	int handleFlags();//处理游戏结束相关flags

	std::vector<JiuMove> replayMoves;//保存历史走步Jiumove
	std::vector<Board> replayBoards;//保存历史局面Board
	std::vector<HashType> replayHashs;//保存历史Hash值

	int GameStage(Board::BoardState stage);//主要循环体，轮流调用两个玩家的getMove函数，Board改变状态后返回
	int currentPlayer = 0;//0-FirstPlayer,1-SecondPlayerpublic:
public:
	


	Game() {}
	Game(Board board):board(board){}
	std::vector<HashType> &getReplayHashs() { return replayHashs; }
	Board getBoard() { return board; }
	std::string getName(ChessState player) { return p[player - 1]->name; }

	void setReplayRules(bool cmd) { replayRulesFlag = cmd; }
	void setPeaceRules(bool cmd) { peaceRulesFlag = cmd; }

	void setChangeBoardFlag() { changeBoardFlag = true; }
	void setUndoFlag() { undoFlag = true; }
	void resetReplayRules() { replayRulesFlag = false; }
	void resetPeaceRules() { peaceRulesFlag = false; }
	void setWriteReplay() { writeReplayFlag = true; }

	template<class T>
	int addPlayer() {
		static_assert(std::is_base_of<Player, T>::value,"Game中添加的玩家类型没有继承于Player基类");
		if (p[0] == nullptr) {
			p[0] = std::unique_ptr<Player>(new T((*this),FIRST_PLAYER));
			std::cout << "p1由"<<p[0]->name<<"操控" << std::endl;
		}
		else if (p[1] == nullptr) {
			p[1] = std::unique_ptr<Player>(new T((*this),SECOND_PLAYER));
			std::cout << "p2由" << p[1]->name << "操控" << std::endl;
		}
		else {
			std::cout << "玩家上限已满";
			return -1;
		}
		return 0;
	}
	/*添加特殊玩家，当使用上面的addPlayer不方便添加特殊玩家时，采用该形式添加。
	同样需要继承自Player，还需要提前将p1，p2的game属性指定为本Game才可以调用*/
	int addPlayPtr(PlayerPtr p1, PlayerPtr p2);

	//语法糖，添加一个人类玩家
	int addHumanPlayer();
	//语法糖，添加一个默认AI
	int addGongJiuPlayer();

	//游戏开始函数，由此调用其他所有函数例如主体循环GameStage，是主要接口
	int startGame();
	//接管录像函数，前期测试用，在后期输出结果已经与startGame的流程有所差别，但还未来得及修正，需要修正后再使用
	int takeOverReplay(JiuMove lastMove);
	//接管残局函数，同样与真正startGame流程有所差别，需要调整后再使用
	int takeOverCanJu(bool useAI1, bool useAI2, ChessState currentPlayer);
	//读谱后按铺面输出，仅支持官方谱，其中函数内部变量skip代表跳过几回合
	int readMode(std::string path);


	//设置棋谱输出位置
	int setReplayFilePath(std::string path);
	//根据历史信息输出棋谱
	int writeReplayToFile();

	//重复探测，检查当前棋局与之前几步的局面是否相同，通过Hash值检测。
	int repeatedDetection();
	~Game();
};

