#pragma once
#include "header.h"

constexpr int Zbsize = 15;
constexpr int ZbLayoutSize = 10;
//置换表
class Zobrist {
public:
	HashType hash[BOARD_SIZE*BOARD_SIZE][2];//每个位置上的棋子对应的Hash值
	int initZobrist();//初始化，给上面数组赋随机值
	TransTable transTable[1 << Zbsize] = { 0 };//置换表
	TransTable layoutTransTable[1 << ZbLayoutSize] = { 0 };//布局置换表
	Zobrist();
	int resetTransTable();//将置换表所有值的深度归零，为了下一次的搜索的新信息能够更好地覆盖旧信息
	int resetLayoutTransTable();//将布局置换表所有值的深度归零
};
extern Zobrist zobrist;