#include "Soldier.h"
#include "Supporter.h"

Soldier::Soldier() : Player()
{
}

Soldier::~Soldier()
{
}

Soldier::Soldier(int team, int id, int type) : Player(team, id)
{
	num_bullets = MAX_BULLETS;
	num_grenades = MAX_GRENADES;
	soldierType = type;
	needMedkit = false;
	needAmmo = false;
}

void Soldier::CalculateTask(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector<Player*>& enemies,
	bool visibillity_map[MSZ][MSZ], Supporter* sp)
{
	if (CheckEnemyInSameRoom(enemies))
		task = SOLDIER_BATTLE_MODE;	// Choosing battle task
	else if (hp < 20)
	{
		if (needMedkit == false)
			task = CALL_FOR_MEDKIT;
		else if (sp->getSoldierProvided() == id)
			task = MOVE_TO_SUPPORTER;
		else
			task = HIDE;
	}
	else if ((num_bullets + num_grenades) < 10)
	{
		if (needAmmo == false)
			task = CALL_FOR_AMMO;
		else if (sp->getSoldierProvided() == id)
			task = MOVE_TO_SUPPORTER;
		else
			task = HIDE;
	}
	else if (soldierType == AGGRESSIVE)
		task = SEARCH_ENEMIES;
	else
		task = FOLLOW_AGGRESSIVE_TEAMMATE;
}

void Soldier::CallForMedkit()
{
	cout << "Soldier " << this->id << " from team " << this->team << " has called for medkit\n";
	needMedkit = true;
}

void Soldier::CallForAmmo()
{
	cout << "Soldier " << this->id << " from team " << this->team << " has called for ammo\n";
	needAmmo = true;
}

// TODO: Try A* to consider enemy distance
void Soldier::RunAway(int maze[MSZ][MSZ], Room* rooms[NUM_ROOMS], double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;

	FindClosestRoom(&trow, &tcol, rooms);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	UpdateNextSoldierStep(maze, next);
}

void Soldier::MoveOnTowardsSupporter(int maze[MSZ][MSZ], Supporter* sp, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow = sp->getRow(), tcol = sp->getCol();

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Making sure that the supporter is the one that handling the recovering
	if (team == RED && maze[next->getRow()][next->getCol()] == RED_SUPPORTER ||
		team == BLUE && maze[next->getRow()][next->getCol()] == BLUE_SUPPORTER)
		return;

	UpdateNextSoldierStep(maze, next);
}

void Soldier::FollowAggressiveTeammate(int maze[MSZ][MSZ], vector<Soldier*>& soldiers, double security_map[MSZ][MSZ])
{
	Cell* next, c;
	int trow, tcol;
	double minDist = HUGE_VAL;

	// Determine target by closest aggressive teammate
	for (auto& sd : soldiers)
		if (sd->getId() != this->id && sd->getSoldierType() == AGGRESSIVE)
			UpdateMinDistCoordinates(row, col, sd->getRow(), sd->getCol(), &trow, &tcol, &minDist);

	if (roomNum != -1) // if not in a tunnel -> find random cell to reach, which closest to aggressive
	{
		priority_queue <Cell, vector<Cell>, CompareCells> followTeam_pq;

		while (followTeam_pq.size() < 3)
		{
			int x, y;
			RandomizePointByRadius(maze, &y, &x, 20);
			followTeam_pq.push(*(new Cell(y, x, trow, tcol, 0, nullptr)));
		}

		c = followTeam_pq.top();
		trow = c.getRow(), tcol = c.getCol();
	}

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	UpdateNextSoldierStep(maze, next);
}

void Soldier::SearchTheEnemies(int maze[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector<Player*>& enemies, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	bool enemyInRoom = false;

	// Setting target to be the closest enemy (wherever he'll located in)
	FindClosestEnemyInMap(&trow, &tcol, enemies);

	// Check for if he isn't in a tunnel
	for (auto& en : enemies)
		if (en->getRow() == trow && en->getCol() == tcol)
			if (en->getRoomNumber() != -1)
				enemyInRoom = true;

	// if enemy not in tunnel, chase him, else - go to another room
	if (!enemyInRoom)
		FindClosestRoom(&trow, &tcol, rooms);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	UpdateNextSoldierStep(maze, next);
}

void Soldier::BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector<Player*>& enemies,
	bool visibillity_map[MSZ][MSZ])
{
	priority_queue<double, vector<double>, less<double>> task_q;
	double hp_per = hp / MAX_HP;
	double ammo_per = (double)(num_bullets + num_grenades) / (double)(MAX_BULLETS + MAX_GRENADES);
	double grenade_per = num_grenades / MAX_GRENADES;
	double dist_per;	// low percent means closer
	double is_visible;
	
	if (hp_per < 0.4 && !needMedkit)
		CallForMedkit();
	if (ammo_per < 0.35 && !needAmmo)
		CallForAmmo();
	
	FindEnemyToFight(&enemy_row, &enemy_col, enemies, visibillity_map, &is_visible);

	dist_per = CalculateEuclideanDistance(row, col, enemy_row, enemy_col) /
		CalculateEuclideanDistance(rooms[roomNum]->getH(), rooms[roomNum]->getW(), 0, 0);

	if (this->soldierType == AGGRESSIVE)
	{
		hp_per *= AGGRESSIVE_HP_W;
		ammo_per *= AGGRESSIVE_AMMO_W;
		dist_per *= AGGRESSIVE_DIST_W;
		is_visible *= AGGRESSIVE_VISIBLE_W;
		grenade_per *= AGGRESSIVE_GRENADES_W;
	}
	else
	{
		hp_per *= CAUTIOUS_HP_W;
		ammo_per *= CAUTIOUS_AMMO_W;
		dist_per *= CAUTIOUS_DIST_W;
		is_visible *= CAUTIOUS_VISIBLE_W;
		grenade_per *= CAUTIOUS_GRENADES_W;
	}

	// Todo: Try combinations
	
	// 0.2 0.5 0.3 1
	// w = { 0.6 0.66 0.7 } --> 0.12 0.33 0.198
	// Closer = 3.737	Attack = 4.137	 Hide = 4.848

	// w = { 0.5 0.5 0.66 } --> 0.375 0.4 0.325
	// Closer = 3.737	Attack = 4.137	 Hide = 4.848

	double getCloser_rate = exp(hp_per) + exp(ammo_per) + exp(dist_per);
	double attack_rate = exp(hp_per) + exp(ammo_per) + (-log(dist_per)) + is_visible + grenade_per;
	double hide_rate = -(log(hp_per) + log(ammo_per) + log(dist_per));

	task_q.push(getCloser_rate);
	task_q.push(attack_rate);
	task_q.push(hide_rate);

	double action = task_q.top();
	task_q.pop();

	// If the enemy invisible and this player have no grenades left, do the next task
	if (action == attack_rate && is_visible == 0 && num_grenades == 0)
		action = task_q.top();

	if (action == getCloser_rate)
		task = GET_CLOSER_TO_ENEMY;
	else if (action == attack_rate)
		AttackEnemy(visibillity_map[enemy_row][enemy_col]);	// Choose the best way to attack
	else
		if (hp <= 10)
			task = RUN_AWAY;
		else
			task = HIDE;
}

void Soldier::GetCloserToEnemy(int maze[MSZ][MSZ], double security_map[MSZ][MSZ])
{
	Cell* next, c;
	int trow = enemy_row, tcol = enemy_col;
	priority_queue <Cell, vector<Cell>, CompareCells> getCloser_pq;

	while (getCloser_pq.size() < 5)
	{
		int x, y;
		RandomizePointByRadius(maze, &y, &x, 20);
		getCloser_pq.push(*(new Cell(y, x, trow, tcol, 0, nullptr)));
	}

	c = getCloser_pq.top();
	trow = c.getRow(), tcol = c.getCol();

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	UpdateNextSoldierStep(maze, next);
}

void Soldier::AttackEnemy(bool isVisible)
{
	isVisible ? task = SHOOT_BULLET : task = THROW_GRENADE;
}

Bullet* Soldier::ShootBullet()
{
	Bullet* pb = new Bullet(col, row, CalculateDirectedAngle(row, col, enemy_row, enemy_col), this->team);
	pb->setIsFired(true);
	num_bullets--;

	return pb;
}

Grenade* Soldier::ThrowGrenade()
{
	Grenade* pg = new Grenade(enemy_row, enemy_col, 
		new Bullet(col, row, CalculateDirectedAngle(row, col, enemy_row, enemy_col), this->team));
	pg->setIsExploded(true);
	num_grenades--;

	return pg;
}

void Soldier::FindClosestEnemyInMap(int *trow, int *tcol, vector<Player*>& enemies)
{
	double minDist = HUGE_VAL;

	for (auto& en : enemies)
		UpdateMinDistCoordinates(row, col, en->getRow(), en->getCol(), trow, tcol, &minDist);
}

void Soldier::FindEnemyToFight(int* trow, int* tcol, vector<Player*>& enemies, bool visibillity_map[MSZ][MSZ],
	double* isVisible)
{
	double minDist = HUGE_VAL;
	vector<Player> visibles;
	vector<Player> sameRoom;

	// find all visibles, if there aren't, gather all enemies in same room
	for (auto& en : enemies)
		if (en->getRoomNumber() == this->roomNum)
			if (visibillity_map[en->getRow()][en->getCol()] == true)
				visibles.push_back(*en);
			else if (visibles.empty())
				sameRoom.push_back(*en);
		
	// find closest visible enemy and flag him as "visible"
	if (!visibles.empty())
		for (auto& en : visibles)
		{
			UpdateMinDistCoordinates(row, col, en.getRow(), en.getCol(), trow, tcol, &minDist);
			*isVisible = 1;
		}
	// find closest enemy in the room and flag him as "not visible"
	else
		for (auto& en : sameRoom)
		{
			UpdateMinDistCoordinates(row, col, en.getRow(), en.getCol(), trow, tcol, &minDist);
			*isVisible = 0;
		}
}

void Soldier::FindClosestRoom(int* trow, int* tcol, Room* rooms[NUM_ROOMS])
{
	double minDist = HUGE_VAL;

	// Determine target by min distance from closest room
	for (int i = 0; i < NUM_ROOMS; i++)
		// Check for if the player is NOT in room[i]
		if (i != this->roomNum)
			UpdateMinDistCoordinates(row, col, rooms[i]->getCenterRow(), rooms[i]->getCenterCol(),
				trow, tcol, &minDist);
}

void Soldier::UpdateNextSoldierStep(int maze[MSZ][MSZ], Cell* next)
{
	if (maze[next->getRow()][next->getCol()] == SPACE)
	{
		row = next->getRow();
		col = next->getCol();
	}
}