#pragma once
#include <time.h>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

// Map Colors
const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int GRAY = 5;
const int WHITE = 6;
const int MED = 7;
const int AMMO = 8;
const int RED_SOLDIER = 10;
const int RED_SUPPORTER = 11;
const int BLUE_SOLDIER = 12;
const int BLUE_SUPPORTER = 13;
const int DEAD = 14;

// Maze
const int MSZ = 100;
const int NUM_SECURITY_MAP_SIMULATIONS = 3;

// Rooms
const int NUM_ROOMS = 10;
const int NUM_MED_ROOMS = 2;
const int NUM_AMMO_ROOMS = 2;
const int MAX_ROOM_WIDTH = MSZ / 5;
const int MAX_ROOM_HEIGHT = MSZ / 5;
const int MIN_ROOM_WIDTH = 10;
const int MIN_ROOM_HEIGHT = 10;
const int SUB_ROOM_SIZE = 5;

// Ammo & Medkits
const int NUM_STOCK = 3;
const double GRENADE_MAX_DIST = 6.0;

// Teams
const int RED = 0;
const int BLUE = 1;

// Players Settings
const int NUM_PLAYERS = 3;
const int NUM_SOLDIERS = 2;
const int NUM_SUPPORTERS = 1;
const int NUM_AGGRESSIVE_SOLDIERS = 1;


static double CalculateEuclideanDistance(int y, int x, int yy, int xx)
{
	return sqrt(pow(y - yy, 2) + pow(x - xx, 2));
}

static double CalculateDirectedAngle(int y, int x, int yy, int xx)
{
	return atan2((yy-y), (xx-x));
}