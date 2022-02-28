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
	Cell* next;
	int trow, tcol;
	priority_queue <Cell, vector<Cell>, CompareCellsBySecurity> pq_target;

	// Determine target by most secured in radius of 10
	while (pq_target.size() < 20)
	{
		int x, y;
		Cell* c;

		do {
			x = rand() % 10;
			y = rand() % 10;
			// For randomize left,right,up,down
			if (rand() % 2)
				x = -x;
			if (rand() % 2)
				y = -y;
			// Now this is the steps size from current supporter position
			x += col;
			y += row;
		} while (maze[y][x] != SPACE);

		c = new Cell(y, x, security_map[y][x]);
		pq_target.push(*c);
	}

	Cell c = pq_target.top();

	next = DistanceFromStartAStar(this->row, this->col, c.getRow(), c.getCol(), maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();
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
	bool targetFound = false;

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
			return RestorePath(pCurrent);

		// paint current black
		blacks.push_back(*pCurrent);
		it_gray = find(grays.begin(), grays.end(), *pCurrent);
		if (it_gray != grays.end()) // current was found
			grays.erase(it_gray); // and removed from grays

		// now check the neighbors of current
		curr_row = pCurrent->getRow();
		curr_col = pCurrent->getCol();

		if (curr_row > 0)	// UP
			CheckNeighbor(pCurrent, curr_row - 1, curr_col, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row - 1, curr_col));
		if (curr_row < MSZ - 1)	// DOWN
			CheckNeighbor(pCurrent, curr_row + 1, curr_col, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row + 1, curr_col));
		if (curr_col < MSZ - 1)	// RIGHT
			CheckNeighbor(pCurrent, curr_row, curr_col + 1, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row, curr_col + 1));
		if (curr_col > 0)	// LEFT
			CheckNeighbor(pCurrent, curr_row, curr_col - 1, pq, grays, blacks,
				CalculateG_BySecurityCost(pCurrent, security_map, curr_row, curr_col - 1));
	}
}

void Player::CheckNeighbor(Cell* pCurrent, int nrow, int ncol,
	priority_queue <Cell, vector<Cell>, CompareCells>& pq,
	vector <Cell>& grays, vector <Cell>& blacks, double g)
{
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	Cell* pn = new Cell(nrow, ncol, pCurrent->getTargetRow(),
		pCurrent->getTargetCol(), g, pCurrent);

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

Cell* Player::RestorePath(Cell* pCurrent)
{
	while (pCurrent->getParent()->getParent() != nullptr)
		pCurrent = pCurrent->getParent();
	return pCurrent;	// returns the next step
}

double Player::CalculateG_BySecurityCost(Cell* pCurrent, double security_map[MSZ][MSZ], int nrow, int ncol)
{
	// G is composed by distance from starting point
	// Plus the cost of not secured cell
	double security_cost = -log(security_map[nrow][ncol]);	// punishing unsecured cells with ln function
	double neighbor_g = pCurrent->getG() + 1;	// distance from starting point + 1

	return neighbor_g + security_cost;
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

bool Player::CheckEnemyInSameRoom(vector<Player> enemies)
{
	for (auto& en : enemies)
		if (en.getRoomNumber() == this->roomNum)
			return true;

	return false;
}

