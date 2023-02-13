#include "pch.h"
#include "evalGenerator.h"


int evalGenerator::getCount()
{
	for (int i = 0; i < 3; i++) {
		count[i] = 0;
	}
	for (int i = 0; i < 4; i++) {
		count[data[i]]++;
	}
	return 0;
}

void evalGenerator::makeEval(int n)
{
	setAllv(0);
	addDatas(n, ed[count[n]]);
	addNonDatas(n, nd[count[n]]);
	return;
}

evalGenerator::evalGenerator()
{
}


evalGenerator::~evalGenerator()
{
}
