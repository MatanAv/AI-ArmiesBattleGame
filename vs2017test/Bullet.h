#pragma once
#include "Definitions.h"

const double MAX_DAMAGE = 100.0;	// Damage is determined by MAX_DAMAGE / distance

class Bullet
{
private:
	int start_row, start_col;
	int hit_row, hit_col;
	int shooterTeam;
	double x, y;
	double angle_direction;
	double distFromHit;
	double hitDamage;
	bool is_fired;
	void CalcHitDamage();
public:
	Bullet();
	Bullet(double xx, double yy);
	Bullet(double xx, double yy, double angle);
	Bullet(double xx, double yy, double angle, int s_team);
	~Bullet();
	bool getIsFired() { return is_fired; }
	int getHitRow() { return hit_row; }
	int getHitCol() { return hit_col; }
	int getShooterTeam() { return shooterTeam; }
	int getX() { return (int)x; }
	int getY() { return (int)y; }
	double getHitDamage() { return hitDamage; } 
	void move(int maze[MSZ][MSZ]);
	void setIsFired(bool status) { is_fired = status; }
	void show();
	void SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage);
	void moveGrenadeBullet(int maze[MSZ][MSZ]);
	bool SimulateFireOnTarget(int maze[MSZ][MSZ], int trow, int tcol);
};

