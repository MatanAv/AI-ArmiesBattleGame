#include "Soldier.h"

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

void Soldier::CalculateTask()
{
}

void Soldier::CallForMedkit()
{
	needMedkit = true;
}

void Soldier::CallForAmmo()
{
	needAmmo = true;
}

// TODO: Try A* to consider enemy distance
// TODO: May should be private
void Soldier::RunAway(int maze[MSZ][MSZ], Room rooms[NUM_ROOMS], double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;

	FindClosestRoom(&trow, &tcol, rooms);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();
}

void Soldier::MoveOnTowardsSupporter(int maze[MSZ][MSZ], vector<Supporter>& supporters, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	bool noSupporterAvailable = true;

	// Determine target by supporter who's in delivery to this soldier
	for (auto& sp : supporters)
		if (sp.getSoldierProvided() == this->id)
		{
			noSupporterAvailable = false;
			trow = sp.getRow();
			tcol = sp.getCol();
			break;
		}

	// If the supporter changed his mission -> In the meantime the soldier will hide
	if (noSupporterAvailable)
		Hide(maze, security_map);
	else
	{
		next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

		// Making sure that the supporter is the one that handling the recovering
		if (team == RED && maze[next->getRow()][next->getCol()] == RED_SUPPORTER ||
			team == BLUE && maze[next->getRow()][next->getCol()] == BLUE_SUPPORTER)
			return;

		// Update new player's location
		row = next->getRow();
		col = next->getCol();
	}
}

// TODO: Not when this player in the room with an enemy
void Soldier::FollowAggressiveTeammate(int maze[MSZ][MSZ], vector<Soldier>& soldiers, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	double minDist = HUGE_VAL;

	// Determine target by closest aggressive teammate
	for (auto& sd : soldiers)
		if (sd.getSoldierType() == AGGRESSIVE)
			UpdateMinDistCoordinates(row, col, sd.getRow(), sd.getCol(), &trow, &tcol, &minDist);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();
}

void Soldier::SearchTheEnemies(int maze[MSZ][MSZ], Room rooms[NUM_ROOMS], vector<Player>& enemies, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	bool enemyInRoom = false;

	// Setting target to be the closest enemy (wherever he'll located in)
	FindClosestEnemyInMap(&trow, &tcol, enemies);

	// Check for if he isn't in a tunnel
	for (auto& en : enemies)
		if (en.getRow() == trow && en.getCol() == tcol)
			if (en.getRoomNumber() != -1)
				enemyInRoom = true;

	// if enemy not in tunnel, chase him, else - go to another room
	if (!enemyInRoom)
		FindClosestRoom(&trow, &tcol, rooms);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();
}

void Soldier::BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room rooms[NUM_ROOMS], vector<Player>& enemies,
	bool visibillity_map[MSZ][MSZ])
{
	priority_queue<double, vector<double>, greater<double>> task_q;
	int trow, tcol;
	double hp_per = hp / MAX_HP;
	double ammo_per = (num_bullets + num_grenades) / (MAX_BULLETS + MAX_GRENADES);
	double grenade_per = num_grenades / MAX_GRENADES;
	double dist_per;	// low percent means closer
	double is_visible;
	
	if (hp_per < 0.4)
		CallForMedkit();
	if (ammo_per < 0.35)
		CallForAmmo();

	FindEnemyToFight(&trow, &tcol, enemies, visibillity_map, &is_visible);

	dist_per = CalculateEuclideanDistance(row, col, trow, tcol) /		
		CalculateEuclideanDistance(rooms[roomNum].getH(), rooms[roomNum].getW(), 0, 0);

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
		GetCloserToEnemy(maze, security_map, trow, tcol);
	else if (action == attack_rate)
		AttackEnemy(trow, tcol, visibillity_map[trow][tcol]);
	else
		if (hp <= 10)
			RunAway(maze, rooms, security_map);
		else
			Hide(maze, security_map);
}

void Soldier::GetCloserToEnemy(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], int trow, int tcol)
{
	Cell* next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();
}

// TODO: fix
void Soldier::AttackEnemy(int trow, int tcol, bool isVisible)
{
	isVisible ? ShootBullet(trow, tcol) : ThrowGrenade(trow, tcol);
}

Bullet* Soldier::ShootBullet(int trow, int tcol)
{
	Bullet* pb = new Bullet(col, row, atan2(trow, tcol));
	pb->setIsFired(true);
	num_bullets--;

	return pb;
}

Grenade* Soldier::ThrowGrenade(int trow, int tcol)
{
	// TODO: Grenade animation maybe should be at main.cpp (idle func)
	
	Grenade* pg = new Grenade((col, row, atan2(trow, tcol));
	pg->setIsExploded(true);
	num_grenades--;

	return pg;
}

void Soldier::FindClosestEnemyInMap(int *trow, int *tcol, vector<Player>& enemies)
{
	double minDist = HUGE_VAL;

	for (auto& en : enemies)
		UpdateMinDistCoordinates(row, col, en.getRow(), en.getCol(), trow, tcol, &minDist);
}

void Soldier::FindEnemyToFight(int* trow, int* tcol, vector<Player>& enemies, bool visibillity_map[MSZ][MSZ],
	double* isVisible)
{
	double minDist = HUGE_VAL;
	vector<Player> visibles;
	vector<Player> sameRoom;

	// find all visibles, if there aren't, gather all enemies in same room
	for (auto& en : enemies)
		if (en.getRoomNumber() == this->roomNum && visibillity_map[en.getRow()][en.getCol()] == true)
			visibles.push_back(en);
		else if (!visibles.empty() && en.getRoomNumber() == this->roomNum)
			sameRoom.push_back(en);

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

void Soldier::FindClosestRoom(int* trow, int* tcol, Room rooms[NUM_ROOMS])
{
	double minDist = HUGE_VAL;

	// Determine target by min distance from closest room
	for (int i = 0; i < NUM_ROOMS; i++)
		// Check for if the player is NOT in room[i]
		if (i != this->roomNum)
			UpdateMinDistCoordinates(row, col, rooms[i].getCenterRow(), rooms[i].getCenterCol(),
				trow, tcol, &minDist);
}
