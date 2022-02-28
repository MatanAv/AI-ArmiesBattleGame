#include "Bullet.h"
//#include <stdlib.h>
#include <math.h>
#include "glut.h"


Bullet::Bullet()
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
}

Bullet::Bullet(double xx, double yy)
{
	angle_direction = (rand() % 360)*3.14 / 180; // in radians
	is_fired = false;
	x = xx;
	y = yy;
	start_row = yy;
	start_col = xx;
}

Bullet::Bullet(double xx, double yy, double angle)
{
	angle_direction = angle; // in radians
	is_fired = false;
	x = xx;
	y = yy;
	start_row = yy;
	start_col = xx;
}

Bullet::Bullet(double xx, double yy, double angle, int s_team)
{
	angle_direction = angle; // in radians
	is_fired = false;
	x = xx;
	y = yy;
	shooterTeam = s_team;
	start_row = yy;
	start_col = xx;
}

Bullet::~Bullet()
{
}


void Bullet::move(int maze[MSZ][MSZ])
{
	// check that current x,y is in SPACE otherwise stop moving bullet
	// Hitted a red player
	if (shooterTeam == BLUE && maze[(int)y][(int)x] == RED_SOLDIER || maze[(int)y][(int)x] == RED_SUPPORTER)
	{
		is_fired = false;
		hit_row = y;
		hit_col = x;
		CalcHitDamage();
	}
	// Hitted a blue player
	else if (shooterTeam == RED && maze[(int)y][(int)x] == BLUE_SOLDIER || maze[(int)y][(int)x] == BLUE_SUPPORTER)
	{
		is_fired = false;
		hit_row = y;
		hit_col = x;
		CalcHitDamage();
	}
	else if (maze[(int)y][(int)x] == WALL)
		is_fired = false;
	if (is_fired)
	{
		double speed = 0.1;
		double dx, dy;
		dx = cos(angle_direction);
		dy = sin(angle_direction);
		// update x,y
		x += dx * speed;
		y += dy * speed;
	}
}

void Bullet::show()
{
	glColor3d(0, 0, 0); // black
	glBegin(GL_POLYGON);
		glVertex2d(x, y + 0.5);
		glVertex2d(x+0.5, y );
		glVertex2d(x, y - 0.5);
		glVertex2d(x-0.5, y );
	glEnd();
	
}

void Bullet::SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], double damage)
{
	while (is_fired) 
	{
		if (maze[(int)y][(int)x] == WALL)
			is_fired = false;
		else
		{
			security_map[(int)y][(int)x] += damage;
			double speed = 0.1;
			double dx, dy;
			dx = cos(angle_direction);
			dy = sin(angle_direction);
			// update x,y
			x += dx * speed;
			y += dy * speed;

		}
	}
}

bool Bullet::SimulateFireOnTarget(int maze[MSZ][MSZ], int trow, int tcol)
{
	while (is_fired)
	{
		if ((int)x == tcol && (int)y == trow)	// bullet has hit the target
		{
			is_fired = false;
			return true;
		}
		else if (maze[(int)y][(int)x] == WALL)	// bullet has hit a wall
		{
			is_fired = false;
			return false;
		}
		else  // bullet is still moving
		{
			double speed = 0.1;
			double dx, dy;
			dx = cos(angle_direction);
			dy = sin(angle_direction);
			// update x,y
			x += dx * speed;
			y += dy * speed;
		}
	}
}

void Bullet::CalcHitDamage()
{
	double shootDist = CalculateEuclideanDistance(start_row, start_col, y, x);
	hitDamage = MAX_DAMAGE / shootDist;
}