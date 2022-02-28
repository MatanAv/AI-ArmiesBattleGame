#pragma once
#include "Cell.h"

class CompareCellsBySecurity
{
public:
	CompareCellsBySecurity();
	~CompareCellsBySecurity();
	bool operator()(Cell& c1, Cell& c2) {
		return c1.getSecurityLevel() < c2.getSecurityLevel();
	}
};

