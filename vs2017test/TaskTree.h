/*
	TaskTree is a class for task decision-making, a kind of decision-tree.
	TaskNode is a condition node:
		- Left child = false/smaller value
		- Right child = true/greater value
	TaskLeaf is a leaf node (which is the decision/task to do).

	Running Best-First-Search on a TaskTree object will provide the most valuable task for each moment.
*/

#pragma once
#include <string>

using namespace std;

class TaskTree
{
private:
	TaskTree* next;
public:
	TaskTree();
	~TaskTree();
	TaskTree* getNextNode() { return next; }
	void setNextNode(TaskTree* n_node) { next = n_node; }
};
