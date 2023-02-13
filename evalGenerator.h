#pragma once
#include <fstream>

class evalGenerator
{
public:
	int data[4] = { 0 };//0 1 2，存储函数中间变量
	int eval[4];//存储中间评价值

	int count[3];//存储中间某时刻，三个状态的数量
	int getCount();

	void makeEval(int n);//评价某个时刻的data，并保存评价值到eval


	int ed[5] = { 0,1,4,16,32 };//2*2范围内，我分子力为0、1、2、3、4时的我方子力评分,EvaluationData
	int nd[5] = { 0,0,4,192,0 };//2*2范围内，我分子力为0、1、2、3、4时的其他子力评分,NotMineEvaluationData
	void rs() {
		for (int i = 0; i < 4; i++)
			eval[i] = 0;
	}
	void setD(int a, int b, int c, int d) {
		data[0] = a;
		data[1] = b;
		data[2] = c;
		data[3] = d;
		getCount();
	}
	void setAllv(int a) {
		for (int i = 0; i < 4; i++)
			eval[i] = a;
	}
	void setv(int a, int b, int c, int d) {
		eval[0] = a;
		eval[1] = b;
		eval[2] = c;
		eval[3] = d;
	}
	void addDatas(int d,int n) {
		for (int i = 0; i < 4; i++)
			if (data[i] == d)
				eval[i] += n;
	}
	void addNonDatas(int d, int n) {
		for (int i = 0; i < 4; i++) {
			if (data[i] != d)
				eval[i] += n;
		}
	}
	
	evalGenerator();
	~evalGenerator();
};

class EG2Text {
public:
	evalGenerator eg;
	void outputEval(int n,std::ofstream& fout) {
		fout << "{";
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 3; b++) {
				for (int c = 0; c < 3; c++) {
					for (int d = 0; d < 3; d++) {
						fout << "{";
						eg.setD(a, b, c, d);
						eg.makeEval(n);
						fout << eg.eval[0];
						for (int i = 1; i < 4; i++) {
							fout << ',' << eg.eval[i];
						}
						fout << "}";
						if (!(a == 2 && b == 2 && c == 2 && d == 2))
							fout << ',';
					}
				}
			}
		}
		fout << "}\n";
	}
	void makeEgFile() {
		std::ofstream fout("eg.txt");
		outputEval(1, fout);
		outputEval(2, fout);

	}
};