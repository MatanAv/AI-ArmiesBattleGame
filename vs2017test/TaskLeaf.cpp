#include "TaskLeaf.h"

TaskLeaf::TaskLeaf(string name, int task) : TaskNode(name)
{
	this->task = task;
}

TaskLeaf::~TaskLeaf()
{
}
