#include "pch.h"
#include "MCTS.h"

TreeNode::TreeNode(Board board):board(board)
{
}

TreeNode::TreeNode(Board board, std::weak_ptr<TreeNode> parent):
	board(board),
	parent(parent)
{
}

TreeNode::~TreeNode()
{
}
/*
MinimaxGameTree::MinimaxGameTree(Board board):
	root(std::make_shared<TreeNode>(board))
{
	
}
*/