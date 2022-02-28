#include "TaskNode.h"

TaskNode::TaskNode(string name)
{
	this->name = name;
}

TaskNode::~TaskNode()
{
}

TaskNode::TaskNode(string name, double value)
{
	this->name = name;
	d_val = value;
	isBoolCondition = false;
}

TaskNode::TaskNode(string name, bool value)
{
	this->name = name;
	b_val = value;
	isBoolCondition = true;
}

void TaskNode::setChildren(TaskNode* l_child, TaskNode* r_child)
{
	if (isBoolCondition)
		if (b_val)
		{
			right_child = r_child;
			r_child->setF(this->f);
			left_child = nullptr;
		}
		else
		{
			left_child = l_child;
			l_child->setF(this->f);
			right_child = nullptr;
		}
	else
	{
		double gain = -log(d_val);	// gain is higher when d_val is low, and lower when d_val is high
		l_child->setF(left_child->getF() + gain);
		r_child->setF(right_child->getF() + (1 - gain));

		left_child = l_child;
		right_child = r_child;
	}
}
