#pragma once
#include "header.h"
#include <string>
#include <iostream>
#include <vector>
/* 
负责输入输出棋谱的类
------
给定一行棋谱，返回一步可执行动作，可用于人类棋手的输入，和棋谱的单步输入

*/
class ManualReader
{
public:
	ManualReader();
	//将两字符坐标表示转为数字坐标,str为输入，xy为输出
	static ChessState Str2Position(std::string str, int & x, int & y);
	//将xy坐标转为字符坐标，输出为ret
	static int Num2Str(int x, int y, ChessState player,char ret[2]);

	
	/* 输入一串字符，输出一个动作
	字符形式为 xx,xx 或 xx-xx,xx,xx,
	*/
	static JiuMove Str2JiuMove(std::string str,ChessState player=NON_PLAYER,MoveState moveState=STEP_PIECE);
	~ManualReader();
};

/* 棋谱播放，读入文件地址，解析后得到两组moves
*/
class ReplayReader {
public:
	int countLayout, countMoves;
	int size;
	std::vector<JiuMove> layoutMoves;
	std::vector<JiuMove> Moves;
	ReplayReader();
	ReplayReader(std::string path);
	int readFile(std::string path);
	JiuMove operator[](int index);
	~ReplayReader();
};

