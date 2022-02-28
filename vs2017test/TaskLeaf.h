#pragma once
#include "TaskNode.h"

class TaskLeaf :
    public TaskNode
{
private:
    int task;
public:
    TaskLeaf(string name, int task);
    ~TaskLeaf();
    int getTask() { return task; }
};

