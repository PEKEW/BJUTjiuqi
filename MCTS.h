#pragma once
#include "Board.h"
#include <memory>

/*
һ��˼·������û��ѡ��˷�������������
*/




/*
���ؿ�����������������AI�µ�һ���࣬
AI���յ�һ����ǰ����BOARD�󣬴�������������
�����������ܶ����ڵ�TreeNode
*/

class TreeNode;
typedef std::shared_ptr<TreeNode> PtrTreeNode;
typedef std::pair<JiuMove, PtrTreeNode> MoveChild;
/*
�������ڵ�
ÿ���ڵ����һ��BOARD��BOARDÿһ��move��Ӧһ�������ڵ�
*/
class TreeNode
{
public:
	Board board;//��ǰ�ڵ��board
	std::vector<MoveChild> moveChilds;//��̽ڵ���
	std::weak_ptr<TreeNode> parent;//���ڵ�

	TreeNode(Board board);
	TreeNode(Board board, std::weak_ptr<TreeNode> parent);
	~TreeNode();

	int CreateChilds();//�ɵ�ǰboard�õ�����move�������ӽڵ�
private:
};


/*
���ؿ���������
������һ�����ڵ㣬����������㷨
alphabeta��֦
*/
class MCTS
{
public:
	PtrTreeNode root;

	int MinimaxGameTree(Board board);

private:
};