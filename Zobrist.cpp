#include "pch.h"
#include "Zobrist.h"

Zobrist zobrist;

typedef  long long U64;
U64 rand64(void) {
	return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

int Zobrist::initZobrist()
{
	for (int i = 0; i < 196; i++) {
		for (int j = 0; j < 2; j++) {
			hash[i][j] = rand64();
		}
	}
	return 0;
}

Zobrist::Zobrist()
{
	initZobrist();
}

int Zobrist::resetTransTable()
{
	for (int i = 0; i < (1 << Zbsize); i++) {
		transTable[i].depth = 0;
	}
	return 0;
}

int Zobrist::resetLayoutTransTable()
{
	for (int i = 0; i < (1 << ZbLayoutSize); i++) {
		layoutTransTable[i].depth = 0;
	}
	return 0;
}
