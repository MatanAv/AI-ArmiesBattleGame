#pragma once
#include "Definitions.h"
#include "Cell.h"
#include "CompareCells.h"
#include "CompareCellsBySecurity.h"
#include "Room.h"
//#include "TaskLeaf.h"

const int MAX_HP = 100;

// Tasks
const int HIDE = 20;

class Player
{
protected:
	int hp, task, team;
	int row, col, id, roomNum;
	bool isAlive;
	//TaskTree* taskDecisionTree;	// Description on github/project-folder !!!
	Cell* DistanceFromStartAStar(int curr_row, int curr_col, int trow, int tcol, int maze[MSZ][MSZ],
		double security_map[MSZ][MSZ]);
	Cell* RestorePath(Cell* pCurrent, int start_row, int start_col);
	double CalculateG_BySecurityCost(Cell* pCurrent, double security_map[MSZ][MSZ], int nrow, int ncol);
	bool CheckEnemyInSameRoom(vector<Player*> enemies);
	void CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
		priority_queue <Cell, vector<Cell>, CompareCells>& pq,
		vector <Cell>& grays, vector <Cell>& blacks, double g);
	void UpdatePQ(priority_queue <Cell, vector<Cell>, CompareCells>& pq, Cell* pn);
	void UpdateMinDistCoordinates(int y, int x, int yy, int xx, int* trow, int* tcol, double* minDist);
public:
	Player();
	~Player();
	Player(const Player& other);
	Player(int team, int id);
	int getHP() { return hp; }
	int getTask() { return task; }
	int getTeamColor() { return team; }
	int getRow() { return row; }
	int getCol() { return col; }
	int getId() { return id; }
	int getRoomNumber() { return roomNum; }
	bool getIsAlive() { return isAlive; }
	void setHP(int hp) { this->hp = hp; }
	void setTask(int t) { task = t; }
	void setRow(int r) { row = r; }
	void setCol(int c) { col = c; }
	void setIsAlive(bool val) { isAlive = val; }
	void setRoomNumber(int i) { roomNum = i; }
	void Hide(int maze[MSZ][MSZ], double security_map[MSZ][MSZ]);
	virtual void CalculateTask() {}
	virtual void BattleMode() {}
	bool operator == (const Player& other) { return other.id == id && other.team == team; }
};
