#pragma once

#include "Bullet.h"

const int NUM_BULLETS = 16;
const double MAX_DISTANCE = 5.0;

class Grenade
{
private:
	Bullet* bullets[NUM_BULLETS];
	double x, y;
	bool isExploded;
public:
	Grenade(double x, double y);
	~Grenade();
	void setIsExploded(bool state);
	bool getIsExploded() { return isExploded; }
	void explode(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ],double security_map[MSZ][MSZ],double damage);
};

