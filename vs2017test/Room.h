#pragma once
#include "Definitions.h"

class Room
{
protected:
	int w, h;
	int center_row, center_col; 
public:
	Room();
	~Room();
	Room(int cr, int cc, int w, int h);
	int getW() { return w; }
	int getH() { return h; }
	int getCenterRow() { return center_row; }
	int getCenterCol() { return center_col; }
};

