#pragma once
#include "header.h"
#include <string>
#include <iostream>
#include <vector>
/* 
��������������׵���
------
����һ�����ף�����һ����ִ�ж������������������ֵ����룬�����׵ĵ�������

*/
class ManualReader
{
public:
	ManualReader();
	//�����ַ������ʾתΪ��������,strΪ���룬xyΪ���
	static ChessState Str2Position(std::string str, int & x, int & y);
	//��xy����תΪ�ַ����꣬���Ϊret
	static int Num2Str(int x, int y, ChessState player,char ret[2]);

	
	/* ����һ���ַ������һ������
	�ַ���ʽΪ xx,xx �� xx-xx,xx,xx,
	*/
	static JiuMove Str2JiuMove(std::string str,ChessState player=NON_PLAYER,MoveState moveState=STEP_PIECE);
	~ManualReader();
};

/* ���ײ��ţ������ļ���ַ��������õ�����moves
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

