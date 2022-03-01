#include "Supporter.h"

Supporter::Supporter() : Player()
{
}

Supporter::~Supporter()
{
}

Supporter::Supporter(int team, int id) : Player(team, id)
{
	num_ammo = MAX_AMMO;
	num_medkits = MAX_MEDKITS;
	soldierToProvide = -1;
	itemProvided = -1;
}

Soldier* Supporter::GetProvidedSoldier(vector<Soldier>& teammates)
{
	for (auto& sd : teammates)
		if (sd.getId() == soldierToProvide)
			return &sd;

	return nullptr;
}

// Todo: Try it
//void Supporter::CalculateTask(vector<Player>& enemies)
//{
//	TaskLeaf* leaf_GoAfterTeam = new TaskLeaf("Follow teammates", FOLLOW_TEAMMATES);
//	TaskLeaf* leaf_UseMed = new TaskLeaf("Use Medkit", USE_MEDKIT);
//	TaskLeaf* leaf_ProvideMed = new TaskLeaf("Provide med to urgent", PROVIDE_MED_TO_URGENT);
//	TaskLeaf* leaf_ProvideAmmo = new TaskLeaf("Provide ammo to urgent", PROVIDE_AMMO_TO_URGENT);
//	TaskLeaf* leaf_FillMed = new TaskLeaf("Fill medkit stock", FILL_MEDKIT_STOCK);
//	TaskLeaf* leaf_FillAmmo = new TaskLeaf("Fill ammo stock", FILL_AMMO_STOCK);
//	TaskLeaf* leaf_BattleMode = new TaskLeaf("Enter Battle Mode", SUPPORTER_BATTLE_MODE);
//
//	TaskNode* cond_hpLevel = new TaskNode("Is HP level low?", (double)hp);
//	TaskNode* cond_medEmpty = new TaskNode("Is med stock empty?", num_medkits == 0);
//	TaskNode* cond_enemySameRoom = new TaskNode("Is enemy in the same room?", CheckEnemyInSameRoom(enemies));
//	TaskNode* cond_soldierNeedHP = new TaskNode("A soldier calling for medkit?",);
//	TaskNode* cond_ammoEmpty = new TaskNode("Is ammo stock empty?", num_ammo == 0);
//	TaskNode* cond_soldierNeedAmmo = new TaskNode("A soldier calling for ammo?",);
//	
//	cond_hpLevel->setChildren(cond_medEmpty, leaf_UseMed);
//	cond_medEmpty->setChildren(cond_enemySameRoom, leaf_FillMed);
//	cond_enemySameRoom->setChildren(cond_soldierNeedHP, leaf_BattleMode);
//	cond_soldierNeedHP->setChildren(cond_ammoEmpty, leaf_ProvideMed);
//	cond_ammoEmpty->setChildren(cond_soldierNeedAmmo, leaf_FillAmmo);
//	cond_soldierNeedAmmo(leaf_GoAfterTeam, leaf_ProvideAmmo);
//
//	taskDecisionTree->setNextNode(cond_medEmpty);
//}

void Supporter::CalculateTask(vector<Player>& enemies, vector<Soldier>& teammates)
{
	if (hp < 20)
		task = USE_MEDKIT;
	else if (num_medkits == 0)
		task = FILL_MEDKIT_STOCK;
	else if (CheckEnemyInSameRoom(enemies))
		task = SUPPORTER_BATTLE_MODE;
	else if (CheckIfSoldierNeedHP(teammates))
		task = PROVIDE_MED_TO_URGENT;
	else if (num_ammo == 0)
		task = FILL_AMMO_STOCK;
	else if (CheckIfSoldierNeedAmmo(teammates))
		task = PROVIDE_AMMO_TO_URGENT;
	else
		task = FOLLOW_TEAMMATES;
}

void Supporter::UseMedkit()
{
	num_medkits--;
	hp = MAX_HP;
	cout << "Player " << this->id << " from team " << this->team << " has used med-kit\n";
}

void Supporter::FillMedkitStock(int maze[MSZ][MSZ], Room* med_rooms[NUM_MED_ROOMS], double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	double minDist = HUGE_VAL;

	// Determine target by min distance from medkit
	for (int i = 0; i < NUM_MED_ROOMS; i++)
		if (maze[med_rooms[i]->getCenterRow()][med_rooms[i]->getCenterCol()] == MED)
			UpdateMinDistCoordinates(row, col, med_rooms[i]->getCenterRow(), med_rooms[i]->getCenterCol(),
				&trow, &tcol, &minDist);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();

	if (row == trow && col == tcol)
	{
		num_medkits = MAX_MEDKITS;
		cout << "Player " << this->id << " from team " << this->team << " has taken a medkit\n";
	}
}

void Supporter::FillAmmoStock(int maze[MSZ][MSZ], Room* ammo_rooms[NUM_AMMO_ROOMS], double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow, tcol;
	double minDist = HUGE_VAL;

	// Determine target by min distance from medkit
	for (int i = 0; i < NUM_AMMO_ROOMS; i++)
		if (maze[ammo_rooms[i]->getCenterRow()][ammo_rooms[i]->getCenterCol()] == AMMO)
			UpdateMinDistCoordinates(row, col, ammo_rooms[i]->getCenterRow(), ammo_rooms[i]->getCenterCol(),
				&trow, &tcol, &minDist);

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	// Update new player's location
	row = next->getRow();
	col = next->getCol();

	if (row == trow && col == tcol)
	{
		num_ammo = MAX_AMMO;
		cout << "Player " << this->id << " from team " << this->team << " has taken an ammo\n";
	}
}

void Supporter::ProvideAmmoToSoldier(int maze[MSZ][MSZ], Soldier* sd, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow = sd->getRow(), tcol = sd->getCol();

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	if (next->getRow() == trow && next->getCol() == tcol)
	{
		sd->setBullets(MAX_BULLETS);
		sd->setGrenades(MAX_GRENADES);
		sd->setNeedAmmo(false);
		cout << "Player " << sd->getId() << " from team " << this->team << " has received ammo\n";
		cout << "Player " << sd->getId() << " from team " << this->team << " current bullets: " << sd->getBullets() << "\n";
		cout << "Player " << sd->getId() << " from team " << this->team << " current grenades: " << sd->getGrenades() << "\n";
		soldierToProvide = -1;
		itemProvided = -1;
	}
}

void Supporter::ProvideMedkitToSoldier(int maze[MSZ][MSZ], Soldier* sd, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow = sd->getRow(), tcol = sd->getCol();

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);

	if (next->getRow() == trow && next->getCol() == tcol)
	{
		sd->setHP(MAX_HP);
		sd->setNeedMedkit(false);
		cout << "Player " << sd->getId() << " from team " << this->team << " has received med-kit\n";
		cout << "Player " << sd->getId() << " from team " << this->team << " current HP: " << sd->getHP() << "\n";
		soldierToProvide = -1;
		itemProvided = -1;
	}
}

void Supporter::BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], vector<Soldier>& teammates)
{
	Soldier* sd = GetProvidedSoldier(teammates);

	if (itemProvided == MED)
		ProvideMedkitToSoldier(maze, sd, security_map);
	else if (itemProvided == AMMO)
		ProvideAmmoToSoldier(maze, sd, security_map);
	else
		Hide(maze, security_map);
}

bool Supporter::CheckIfSoldierNeedHP(vector<Soldier>& teammates)
{
	if (soldierToProvide > -1 && itemProvided == MED)
		return true;
	
	// else - check if someone need better and who's urgent
	int minHP = HUGE_VAL;
	for (auto& sd : teammates)
		if (sd.getNeedMedkit() && sd.getHP() < minHP)
		{
			soldierToProvide = sd.getId();
			minHP = sd.getHP();
		}
	
	if (soldierToProvide < 0)	// None of the soldiers need medkit
		return false;

	itemProvided = MED;
	return true;
}

bool Supporter::CheckIfSoldierNeedAmmo(vector<Soldier>& teammates)
{
	if (soldierToProvide > -1 && itemProvided == MED)
		return true;

	// else - check if someone need better and who's urgent
	int minAmmo = HUGE_VAL;
	for (auto& sd : teammates)
		if (sd.getNeedAmmo() && (sd.getBullets() + sd.getGrenades()) < minAmmo)
		{
			soldierToProvide = sd.getId();
			minAmmo = sd.getBullets() + sd.getGrenades();
		}

	if (soldierToProvide < 0)	// None of the soldiers need medkit
		return false;

	itemProvided = AMMO;
	return true;
}

void Supporter::FollowTeammates(int maze[MSZ][MSZ], vector<Soldier>& soldiers, double security_map[MSZ][MSZ])
{
	Cell* next;
	int trow = 0, tcol = 0;

	for (auto &s : soldiers)
	{
		trow += s.getCol();
		tcol += s.getRow();
	}

	// Target is the average cell, between all of the soldiers
	trow /= NUM_SOLDIERS;
	tcol /= NUM_SOLDIERS;

	next = DistanceFromStartAStar(this->row, this->col, trow, tcol, maze, security_map);
	
	// Update new player's location
	row = next->getRow();
	col = next->getCol();
}
