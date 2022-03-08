#pragma once

#include "Bullet.h"

const int NUM_BULLETS = 16;

class Grenade
{
private:
	Bullet* bullets[NUM_BULLETS];
	double x, y;
	bool isExploded;
	void UpdateExplosionState();
public:
	Grenade(double x, double y);
	Grenade(double x, double y, int thrower_color);
	~Grenade();
	Bullet** getBullets() { return bullets; }
	int getX() { return (int)x; }
	int getY() { return (int)y; }
	bool getIsExploded() { return isExploded; }
	void setIsExploded(bool state);
	void explode(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage);
};

