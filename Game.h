#pragma once
#include "Board.h"
#include "Player.h"
#include "ManualReader.h"
#include <memory>


typedef std::unique_ptr<Player> PlayerPtr;
class Game
{
	Board board;
	ReplayReader replay;//������
	std::array<PlayerPtr,2> p = { nullptr,nullptr };//�������ָ�룬p[0]���������

	JiuMove lastMove=JiuMove::errorJiuMove();//������һ��

	bool writeReplayFlag = false;//�Ƿ��������
	std::string filePath;//������׵��ļ�λ��

	bool replayRulesFlag = true;//�Ƿ������ظ��и�����
	bool peaceRulesFlag = true;//�Ƿ����ó�ʱ�䲻������ƽ����

	bool changeBoardFlag = false;//�޸ľ��溯����־�������
	bool undoFlag = false;//������һ���������

	bool repeatedDrawFlag = false;//�ظ��ж�ƽ�֣��ڲ�����
	bool peaceDrawFlag = false;//�޳����ж�ƽ�֣��ڲ�����

	int handleFlags();//������Ϸ�������flags

	std::vector<JiuMove> replayMoves;//������ʷ�߲�Jiumove
	std::vector<Board> replayBoards;//������ʷ����Board
	std::vector<HashType> replayHashs;//������ʷHashֵ

	int GameStage(Board::BoardState stage);//��Ҫѭ���壬��������������ҵ�getMove������Board�ı�״̬�󷵻�
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
		static_assert(std::is_base_of<Player, T>::value,"Game����ӵ��������û�м̳���Player����");
		if (p[0] == nullptr) {
			p[0] = std::unique_ptr<Player>(new T((*this),FIRST_PLAYER));
			std::cout << "p1��"<<p[0]->name<<"�ٿ�" << std::endl;
		}
		else if (p[1] == nullptr) {
			p[1] = std::unique_ptr<Player>(new T((*this),SECOND_PLAYER));
			std::cout << "p2��" << p[1]->name << "�ٿ�" << std::endl;
		}
		else {
			std::cout << "�����������";
			return -1;
		}
		return 0;
	}
	/*���������ң���ʹ�������addPlayer����������������ʱ�����ø���ʽ��ӡ�
	ͬ����Ҫ�̳���Player������Ҫ��ǰ��p1��p2��game����ָ��Ϊ��Game�ſ��Ե���*/
	int addPlayPtr(PlayerPtr p1, PlayerPtr p2);

	//�﷨�ǣ����һ���������
	int addHumanPlayer();
	//�﷨�ǣ����һ��Ĭ��AI
	int addGongJiuPlayer();

	//��Ϸ��ʼ�������ɴ˵����������к�����������ѭ��GameStage������Ҫ�ӿ�
	int startGame();
	//�ӹ�¼������ǰ�ڲ����ã��ں����������Ѿ���startGame������������𣬵���δ���ü���������Ҫ��������ʹ��
	int takeOverReplay(JiuMove lastMove);
	//�ӹܲоֺ�����ͬ��������startGame�������������Ҫ��������ʹ��
	int takeOverCanJu(bool useAI1, bool useAI2, ChessState currentPlayer);
	//���׺������������֧�ֹٷ��ף����к����ڲ�����skip�����������غ�
	int readMode(std::string path);


	//�����������λ��
	int setReplayFilePath(std::string path);
	//������ʷ��Ϣ�������
	int writeReplayToFile();

	//�ظ�̽�⣬��鵱ǰ�����֮ǰ�����ľ����Ƿ���ͬ��ͨ��Hashֵ��⡣
	int repeatedDetection();
	~Game();
};

