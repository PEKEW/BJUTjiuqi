#pragma once
#include "header.h"

constexpr int Zbsize = 15;
constexpr int ZbLayoutSize = 10;
//�û���
class Zobrist {
public:
	HashType hash[BOARD_SIZE*BOARD_SIZE][2];//ÿ��λ���ϵ����Ӷ�Ӧ��Hashֵ
	int initZobrist();//��ʼ�������������鸳���ֵ
	TransTable transTable[1 << Zbsize] = { 0 };//�û���
	TransTable layoutTransTable[1 << ZbLayoutSize] = { 0 };//�����û���
	Zobrist();
	int resetTransTable();//���û�������ֵ����ȹ��㣬Ϊ����һ�ε�����������Ϣ�ܹ����õظ��Ǿ���Ϣ
	int resetLayoutTransTable();//�������û�������ֵ����ȹ���
};
extern Zobrist zobrist;