// JiuQi.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include <array>
#include "Board.h"
#include "ManualReader.h"
#include "Game.h"
#include "Minimax.h"
#include "evalGenerator.h"
#include "GongJiuPlayer.h"
#include <Windows.h>
#include <fileapi.h>
void BoardTest() {
	Board b;
	b.draw();
}

void MRTest() {
	ManualReader mr;
	std::string a("G8-G6,g7,e6");
	//std::string a;
	//std::cin >> a;
	std::cout << a;
	JiuMove j=mr.Str2JiuMove(a);
}

void MRTEST2() {
	//测试Str2Position
	std::string a("e8");
	ManualReader mr;
	int x, y;
	mr.Str2Position(a,x,y);
	std::cout << x<<' ' << y;
}

void JiumoveTest() {
	JiuMove j(1, 2, 3, 4);
	for (int i = 1; i < 12; i++)
		j.addJumpPos(3, 5);
}

void TimeTest() {
	Board*b;
	clock_t mem_start = clock();
	for (int i = 0; i < 1000000; i++) {
		b=new Board;
	}
	clock_t mem_end = clock();
	std::cout << (mem_end - mem_start);

}
template<class GongJiu>
int gameTest(bool isBot1,bool isBot2) {
	Game g;
	//g.addHumanPlayer();
	if (isBot1)
		g.addPlayer<GongJiu>();
	else
		g.addHumanPlayer();
	if (isBot2)
		g.addPlayer<GongJiu>();
	else
		g.addHumanPlayer();
	g.setPeaceRules(false);
	g.setReplayRules(false);
	int ret = 0;

	char filename[255];
	tm* tmp = new tm;
	time_t now;
	now = time(0); // get current time
	localtime_s(tmp, &now); // get structure
	CreateDirectoryA("HumanVsBotsData", NULL);
	sprintf_s(filename, 255, "HumanVsBotsData\\%dx%d_%sVs%s_%04d%02d%02d_%02d_%02d_%02d.txt", 
		BOARD_SIZE, BOARD_SIZE,
		g.getName(FIRST_PLAYER).c_str(), g.getName(SECOND_PLAYER).c_str(),
		tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	delete(tmp);
	g.setReplayFilePath(filename);
	g.setWriteReplay();

	ret=g.startGame();
	return ret;
}
int replayTest() {
	Game g;
	clock_t mem_start = clock();
	g.readMode("save\\replay1.txt");
	clock_t mem_end = clock();
	std::cout <<std::endl<< (double)(mem_end - mem_start)/CLOCKS_PER_SEC;
	return 0;
}
int vectorTest() {
	std::vector<int> v;
	std::cout << v.size() << std::endl;
	v.push_back(3);
	std::cout << v.size();
	return 0;
}
int fileTest() {
	std::ofstream file;
	file.open("save\\replay1.txt");
	std::cout << file.is_open();
	file << "hello";
	file.close();
	std::ifstream f("save\\replay1.txt");
	std::string s;
	f >> s;
	std::cout << s;
	return 0;
}

int arrayTest() {
	std::vector<std::array<int, 2>> s;
	s.push_back({ 1,2 });
	std::cout << s[0][1];
	return 0;
}
int vectorTest2() {
	struct T {
		std::vector<int> v;
	};
	T t1;
	t1.v.push_back(1);
	T t2 = t1;
	t2.v.push_back(2);
	std::cout << t1.v.size() <<" "<< t2.v.size();
	return 0;
}
int vectorSortTest() {
	class T {
	public:
		bool operator< (const T &cmp)  const {
			return (a + b) < (cmp.a + cmp.b);
		}

		int a;
		int b;
		T(int a, int b) :a(a), b(b) {}

	};
	std::vector<T> lis;
	for (int i = 10; i > 1; i--) {
		lis.push_back(T(i, i));
	}
	std::sort(lis.begin(), lis.end());
	for (auto iter : lis) {
		std::cout << iter.a << '\t'<<iter.b << std::endl;
	}
	return 0;
}

int EGTest() {

	EG2Text e;
	e.makeEgFile();
	return 0;
}
int RightShiftTest() {
	unsigned long long a = ULLONG_MAX;
	std::cout << a << '\n';
	for (int i = 0; i < 64; i++) {
		unsigned long long b = a >> i;
		std::cout <<"右移"<<i<<"位:" <<b << '\n';
		int c = a>>i;
		std::cout << "右移" << i << "位:" << c << '\n';
	}//54位=1024
	return 0;
}

int HHTest() {
	using namespace std;
	ifstream fin("input.txt"); // 已有输入文件
	//ofstream fout("output.txt"); //输出文件
	streambuf *cinbackup;
	//streambuf *coutbackup;
	cinbackup = cin.rdbuf(fin.rdbuf()); //用 rdbuf() 重新定向
	//coutbackup = cout.rdbuf(fout.rdbuf()); //用 rdbuf() 重新定向

	Game g;
	g.addHumanPlayer();
	g.addHumanPlayer();
	int ret = 0;
	ret = g.startGame();

	cin.rdbuf(cinbackup); // 取消，恢复键盘输入
	//cout.rdbuf(coutbackup); //取消，恢复屏幕输出
	return ret;
}

int MiniTest(int p) {
	Board b;
	std::ifstream fin("Binput.txt");
	std::vector<HashType> hs;
	char a;
	b.boardState = Board::MOVE;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			fin >> a;
			if (a == 'o')
				b.addPieceState(i, j, SECOND_PLAYER);
			else if (a == '*')
				b.addPieceState(i, j, FIRST_PLAYER);
		}
	}
	b.draw();
	Minimax m(b,(ChessState)p,hs);
	JiuMove mv = m.ABsearch();
	b.doMoves(mv);
	b.drawWithMove(mv);

	return 0;
}

int canJuTest() {
	Board b;
	std::ifstream fin("Binput.txt");
	char a;
	b.boardState = Board::FLY;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			fin >> a;
			if (a == 'o')
				b.addPieceState(i, j, SECOND_PLAYER);
			else if (a == '*')
				b.addPieceState(i, j, FIRST_PLAYER);
		}
	}
	Game g(b);
	g.takeOverCanJu(1,0,SECOND_PLAYER);
	return 0;
}

int sizeTest() {
	std::cout << "sizeof(Board):" << sizeof(Board) << std::endl
		<< "sizeof(JiuMove):" << sizeof(JiuMove) << std::endl
		<< "sizeof(Game):" << sizeof(Game) << std::endl
		<< "sizeof(int):" << sizeof(int) << std::endl
		<< "sizeof(HashType):" << sizeof(HashType) << std::endl;
		//<< "sizeof(zobrist):" << sizeof(zobrist) << std::endl;
	return 0;
}
struct Record {
	int countTotal = 0;
	int countWinP1 = 0;
	int countWinP2 = 0;
	int countDraw = 0;
	bool recordName = false;
	std::string nameP1;
	std::string nameP2;
	std::vector<int> winnerList;
	std::vector<Board> enddingBoard;
	std::vector<bool> changeOrders;
	friend std::ostream & operator<<(std::ostream &ost, Record &r) {
		ost << "总共比赛" << r.countTotal << "场\n"
			<< r.nameP1<<"胜利" << r.countWinP1 << "场\n"
			<< r.nameP2<<"胜利" << r.countWinP2 << "场\n"
			<< "平局" << r.countDraw << "场\n";
		ost << "胜场情况";
		for (int i = 0; i < r.winnerList.size(); i++) {
			if (r.changeOrders[i])
				ost << 'C';
			else
				ost << 'N';
			ost << r.winnerList[i] << " ";
		}
		ost << "\n";
		for (auto item : r.enddingBoard) {
			ost << item;
		}
		return ost;
	}
};

template<class GongJiuF,class GongJiuS>
int BotVsBot() {
	char filename[255];
	tm* tmp=new tm;
	time_t now;
	now = time(0); // get current time
	localtime_s(tmp,&now); // get structure
	Record record;
	{
		Game g;
		g.addPlayer<GongJiuF>();
		g.addPlayer<GongJiuS>();
		record.nameP1 = g.getName(FIRST_PLAYER);
		record.nameP2 = g.getName(SECOND_PLAYER);
		record.recordName = true;
	}
	CreateDirectoryA("BotsVsBotsData", NULL);;
	sprintf_s(filename,255, "BotsVsBotsData\\%sVs%s_%04d%02d%02d_%02d_%02d_%02d.txt",record.nameP1.c_str() ,record.nameP2.c_str(),
		tmp->tm_year + 1900,tmp->tm_mon + 1,tmp->tm_mday, 
		tmp->tm_hour,tmp->tm_min,tmp->tm_sec);
	delete(tmp);
	std::ofstream result(filename);
	result << "File Test";
	result.close();
	for (int i = 0; i < 10; i++) {
		bool changeOrder = (i % 2 == 1);//false 等于不换，true=换
		Game g;
		if (!changeOrder) {
			g.addPlayer<GongJiuF>();
			g.addPlayer<GongJiuS>();
		}
		else {
			g.addPlayer<GongJiuS>();
			g.addPlayer<GongJiuF>();
		}
		int ret = g.startGame();
		record.winnerList.push_back(ret);
		record.enddingBoard.push_back(g.getBoard());
		record.changeOrders.push_back(changeOrder);
		if(ret==0){
			record.countDraw++;
		}
		else {
			bool P1win = !(changeOrder == 0) ^ (ret == 1);
			record.countWinP1 += P1win;
			record.countWinP2 += (1 - P1win);
		}
		record.countTotal++;
		std::ofstream result(filename);
		result << record;
		result.close();
	}
	return 0;
}

//LayoutRulesBot vs LayoutRulesBot
int LBRvLBR(int r1,int r2) {	
	char filename[255];
	tm* tmp = new tm;
	time_t now;
	now = time(0); // get current time
	localtime_s(tmp, &now); // get structure
	Record record;
	{
		Game g;
		PlayerPtr p1 = std::unique_ptr<Player>(new WeightEvalGongJiuPlayerFirst(g,NON_PLAYER, r1));
		PlayerPtr p2 = std::unique_ptr<Player>(new WeightEvalGongJiuPlayerFirst(g,NON_PLAYER, r2));
		g.addPlayPtr(std::move(p1), std::move(p2));
		record.nameP1 = g.getName(FIRST_PLAYER);
		record.nameP2 = g.getName(SECOND_PLAYER);
	}
	CreateDirectoryA("BotsVsBotsData", NULL);
	sprintf_s(filename, 255, "BotsVsBotsData\\%sVs%s_%04d%02d%02d_%02d_%02d_%02d.txt", record.nameP1.c_str(), record.nameP2.c_str(),
		tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	delete(tmp);
	std::ofstream result(filename);
	result << "File Test";
	result.close();
	for (int i = 0; i < 10; i++) {
		bool changeOrder = (i % 2 == 1);
		Game g;
		PlayerPtr p1 = std::unique_ptr<Player>(new WeightEvalGongJiuPlayerFirst(g, NON_PLAYER, r1));
		PlayerPtr p2 = std::unique_ptr<Player>(new WeightEvalGongJiuPlayerFirst(g, NON_PLAYER, r2));
		
		if (!changeOrder)
			g.addPlayPtr(std::move(p1), std::move(p2));
		else
			g.addPlayPtr(std::move(p2), std::move(p1));
		
		int ret = g.startGame();
		record.winnerList.push_back(ret);
		record.enddingBoard.push_back(g.getBoard());
		record.changeOrders.push_back(changeOrder);
		if (ret == 0) {
			record.countDraw++;
		}
		else {
			bool P1win = (changeOrder) ^ (ret == 1);
			record.countWinP1 += P1win;
			record.countWinP2 += (1 - P1win);
		}
		record.countTotal++;
		std::ofstream result(filename);
		result << record;
		result.close();
	}
	return 0;
}

int EvalTest() {
	Board b;
	std::ifstream fin("Einput.txt");
	char a;
	b.boardState = Board::FLY;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			fin >> a;
			if (a == 'o')
				b.addPieceState(i, j, SECOND_PLAYER);
			else if (a == '*')
				b.addPieceState(i, j, FIRST_PLAYER);
		}
	}
	int value=b.evalLayoutMethodFirst(true);
	std::cout << value<<"\n";

	Board b2;
	b2.boardState = Board::FLY;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			fin >> a;
			if (a == 'o')
				b2.addPieceState(i, j, SECOND_PLAYER);
			else if (a == '*')
				b2.addPieceState(i, j, FIRST_PLAYER);
		}
	}
	value = b2.evalLayoutMethodFirst(true);
	std::cout << value;

	return 0;
}


int championshipsMainFunction() {
	std::cout << "是否为我方AI先手,请输入数字，1-是,0-否\n";
	std::string s;
	std::cin >> s;
	if(s[0]=='1')
		gameTest<GongJiuPlayer>(1, 0);
	else
		gameTest<GongJiuPlayer>(0, 1);
	return 0;
}
int main()
{
	//replayTest();
	//MiniTest(2);
	//EGTest();
	//gameTest<GongJiuPlayer>(1,1);
	//canJuTest();
	//sizeTest();
	BotVsBot<GongJiuPlayer, GongJiuPlayer>();
	//LBRvLBR(0, 3);
	//championshipsMainFunction();
	//EvalTest();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
