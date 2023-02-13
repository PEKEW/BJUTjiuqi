#pragma once
#include "Board.h"
#include <memory>

/*
一个思路，最终没有选择此方案，可以无视
*/




/*
蒙特卡洛搜索树，本身是AI下的一个类，
AI接收到一个当前局面BOARD后，传入该类进行搜索
搜索树包含很多树节点TreeNode
*/

class TreeNode;
typedef std::shared_ptr<TreeNode> PtrTreeNode;
typedef std::pair<JiuMove, PtrTreeNode> MoveChild;
/*
搜索树节点
每个节点包含一个BOARD，BOARD每一个move对应一个下属节点
*/
class TreeNode
{
public:
	Board board;//当前节点的board
	std::vector<MoveChild> moveChilds;//后继节点们
	std::weak_ptr<TreeNode> parent;//父节点

	TreeNode(Board board);
	TreeNode(Board board, std::weak_ptr<TreeNode> parent);
	~TreeNode();

	int CreateChilds();//由当前board得到可行move，生成子节点
private:
};


/*
蒙特卡洛搜索树
包含有一个根节点，及搜索相关算法
alphabeta剪枝
*/
class MCTS
{
public:
	PtrTreeNode root;

	int MinimaxGameTree(Board board);

private:
};