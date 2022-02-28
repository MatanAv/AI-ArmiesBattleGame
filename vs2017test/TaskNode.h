#pragma once
#include "TaskTree.h"

class TaskNode :
	public TaskTree
{
protected:
	string name;
	TaskNode* left_child;
	TaskNode* right_child;
	double f, d_val;
	bool b_val;
	bool isBoolCondition;
public:
	TaskNode(string name);
	~TaskNode();
	TaskNode(string name, double value);
	TaskNode(string name, bool value);
	void setChildren(TaskNode* l_child, TaskNode* r_child);
	void setF(double val) { f = val; }
	double getF() { return f; }
};

