#include "Room.h"



Room::Room()
{
}


Room::~Room()
{
}

Room::Room(int cr, int cc, int w, int h)
{
	this->w = w;
	this->h = h;
	center_row = cr;
	center_col = cc;
}
