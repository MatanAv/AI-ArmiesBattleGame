#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

Player::Player(const Player& other)
{
	hp = other.hp;
	task = other.task;
	team = other.team;
	row = other.row;
	col = other.col;
	id = other.id;
	isAlive = other.isAlive;
}

Player::Player(int team, int id)
{
	hp = MAX_HP;
	this->team = team;
	this->id = id;
	isAlive = true;
	roomNum = -1;

	srand(time(0)); // initialize random seed
}

// TODO: Try A* to consider enemy distance
// TODO: May should be private
void Player::Hide(int maze[MSZ][MSZ], double security_map[MSZ][MSZ])
{
	Cell* next, c;
	int trow, tcol;
	priority_queue <Cell, vector<Cell>, CompareCellsBySecurity> pq_target;

	// Determine target by most secured in radius of 10
	while (pq_target.size() < 20)
	{
		int x, y;
		do {
			RandomizePointByRadius(maze, &y, &x, 10);
		} while (maze[y][x] != SPACE);

		pq_target.push(*(new Cell(y, x, security_map[y][x])));
	}

	c = pq_target.top();

	next = DistanceFromStartAStar(this->row, this->col, c.getRow(), c.getCol(), maze, security_map);

	if (maze[next->getRow()][next->getCol()] == SPACE)
	{
		row = next->getRow();
		col = next->getCol();
	}
}

Cell* Player::DistanceFromStartAStar(int curr_row, int curr_col, int trow, int tcol, int maze[MSZ][MSZ],
	double security_map[MSZ][MSZ])
{
	vector <Cell> grays;
	vector <Cell> blacks;
	priority_queue <Cell, vector<Cell>, CompareCells> pq;
	Cell* pstart = new Cell(curr_row, curr_col, trow, tcol, 0, nullptr,
		security_map[curr_row][curr_col]);
	Cell* pCurrent;

	// initializes grays and pq
	grays.push_back(*pstart);
	pq.push(*pstart);
	vector<Cell>::iterator it_gray;

	while (!pq.empty())
	{
		pCurrent = new Cell(pq.top());
		pq.pop();

		// If current is actually a target then we stop A*
		if (pCurrent->getRow() == trow && pCurrent->getCol() == tcol) // then it is target
			return RestorePath(pCurrent, pstart->getRow(), pstart->getCol());

		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent);
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays

		// now check the neighbors of current
		curr_row = pCurrent->getRow();
		curr_col = pCurrent->getCol();

		bool up = false, down = false,
			right = false, left = false;

		CheckStepsDirection(maze, curr_row, curr_col, &up, &down, &right, &left);

		if (up)
			CheckNeighbor(pCurrent, curr_row - 1, curr_col, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row - 1, curr_col), security_map);
		if (down)
			CheckNeighbor(pCurrent, curr_row + 1, curr_col, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row + 1, curr_col), security_map);
		if (right)
			CheckNeighbor(pCurrent, curr_row, curr_col + 1, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row, curr_col + 1), security_map);
		if (left)
			CheckNeighbor(pCurrent, curr_row, curr_col - 1, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row, curr_col - 1), security_map);
	}
}

void Player::CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells>& pq,
	vector <Cell>& grays, vector <Cell>& blacks, double g, double security_map[MSZ][MSZ])
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	Cell* pn = new Cell(nrow, ncol, pCurrent->getTargetRow(),
		pCurrent->getTargetCol(), g, pCurrent, security_map[nrow][ncol]);

	// check the color of this neighbor
	it_black = find(blacks.begin(), blacks.end(), *pn);
	if (it_black != blacks.end()) // it was found i.e. it is black
		return;

	// white
	it_gray = find(grays.begin(), grays.end(), *pn);
	if (it_gray == grays.end()) // it wasn't found => it is white
	{
		grays.push_back(*pn); // paint it gray
		pq.push(*pn);
	}
	else // it is gray
	{
		//   new F            old F
		if (pn->getF() < it_gray->getF()) // then update it (F of neighbor)!!!
		{
			// we need to update it in two places:
			// 1. in vector grays
			it_gray->setG(pn->getG());
			it_gray->setF(pn->getF());
			// 2. in PQ
			// to update a Cell in pq we need to remove it from pq, to update it and to push it back
			UpdatePQ(pq, pn);
		}
	}
}

void Player::UpdatePQ(priority_queue <Cell, vector<Cell>, CompareCells>& pq, Cell* pn)
{
	vector<Cell> tmp;
	Cell tmpCell;
	do {
		if (pq.empty())
		{
			cout << "ERROR! PQ is empty in update PQ\n";
			exit(1);
		}
		tmpCell = pq.top();
		pq.pop();
		if (!(tmpCell == (*pn)))
			tmp.push_back(tmpCell);
	} while (!(tmpCell == (*pn)));

	// now we are aout of do-while because we have found the neighbor in PQ. So change it to *pn.
	pq.push(*pn);

	// now push back all the elements that are in tmp
	while (!tmp.empty())
	{
		pq.push(tmp.back());
		tmp.pop_back();
	}
}

Cell* Player::RestorePath(Cell* pCurrent, int start_row, int start_col)
{
	// In case player won't move this step
	if (pCurrent->getRow() == start_row && pCurrent->getCol() == start_col)
		return pCurrent;

	while (pCurrent->getParent()->getRow() != start_row ||
		pCurrent->getParent()->getCol() != start_col)
		pCurrent = pCurrent->getParent();

	return pCurrent;	// returns the next step
}

double Player::CalculateG_BySecurityCost(Cell* pCurrent, double security_map[MSZ][MSZ], int nrow, int ncol)
{
	// G is composed by distance from starting point
	// Plus the cost of not secured cell
	//double security_cost = -log(security_map[nrow][ncol] + 0.01);	// punishing unsecured cells with ln function
	//double security_cost = pCurrent->getSecurityLevel();
	double neighbor_g = pCurrent->getG() + 1;	// distance from starting point + 1

	//return neighbor_g + (0.05 * security_cost);
	return neighbor_g;
	//return security_cost;
	//return 0;
}

void Player::UpdateMinDistCoordinates(int y, int x, int yy, int xx, int* trow, int* tcol, double* minDist)
{
	double dist = CalculateEuclideanDistance(y, x, yy, xx);
	if (dist < *minDist)
	{
		*minDist = dist;
		*trow = yy;
		*tcol = xx;
	}
}

void Player::RandomizePointByRadius(int maze[MSZ][MSZ], int* r_row, int* r_col, int radius)
{
	int x, y;
	do {
		x = rand() % radius;
		y = rand() % radius;
		// For randomize left,right,up,down
		if (rand() % 2)
			x = -x;
		if (rand() % 2)
			y = -y;
		// Now this is the steps size from current supporter position
		x += col;
		y += row;
	} while (maze[y][x] != SPACE);

	*r_row = y;
	*r_col = x;
}

bool Player::CheckEnemyInSameRoom(vector<Player*> enemies)
{
	if (this->roomNum == -1)
		return false;

	for (auto& en : enemies)
		if (en->getRoomNumber() != -1 && en->getRoomNumber() == this->roomNum)
			return true;

	return false;
}

void Player::CheckStepsDirection(int maze[MSZ][MSZ], int c_row, int c_col, bool* up, bool* down, bool* right, bool* left)
{
	*up = c_row > 0 && maze[c_row - 1][c_col] == SPACE ||
		maze[c_row - 1][c_col] == MED ||
		maze[c_row - 1][c_col] == AMMO ||
		maze[c_row - 1][c_col] == RED_SOLDIER ||
		maze[c_row - 1][c_col] == RED_SUPPORTER ||
		maze[c_row - 1][c_col] == BLUE_SOLDIER ||
		maze[c_row - 1][c_col] == BLUE_SUPPORTER;

	*down = c_row < MSZ - 1 && maze[c_row + 1][c_col] == SPACE ||
		maze[c_row + 1][c_col] == MED ||
		maze[c_row + 1][c_col] == AMMO ||
		maze[c_row + 1][c_col] == RED_SOLDIER ||
		maze[c_row + 1][c_col] == RED_SUPPORTER ||
		maze[c_row + 1][c_col] == BLUE_SOLDIER ||
		maze[c_row + 1][c_col] == BLUE_SUPPORTER;
		
	*right = c_col < MSZ - 1 && maze[c_row][c_col + 1] == SPACE ||
		maze[c_row][c_col + 1] == MED ||
		maze[c_row][c_col + 1] == AMMO ||
		maze[c_row][c_col + 1] == RED_SOLDIER ||
		maze[c_row][c_col + 1] == RED_SUPPORTER ||
		maze[c_row][c_col + 1] == BLUE_SOLDIER ||
		maze[c_row][c_col + 1] == BLUE_SUPPORTER;
	
	*left = c_col > 0 && maze[c_row][c_col - 1] == SPACE ||
		maze[c_row][c_col - 1] == MED ||
		maze[c_row][c_col - 1] == AMMO ||
		maze[c_row][c_col - 1] == RED_SOLDIER ||
		maze[c_row][c_col - 1] == RED_SUPPORTER ||
		maze[c_row][c_col - 1] == BLUE_SOLDIER ||
		maze[c_row][c_col - 1] == BLUE_SUPPORTER;
}

