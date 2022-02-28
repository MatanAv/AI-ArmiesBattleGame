#pragma once
#include "Player.h"
#include "Supporter.h"
#include "Bullet.h"
#include "Grenade.h"

const int MAX_BULLETS = 30;
const int MAX_GRENADES = 3;
const int AGGRESSIVE = 0;
const int COVERING = 1;

// Todo: Try different combinations
// Low weight --> more cautious, High weight --> more risky
const double AGGRESSIVE_HP_W = 0.7;
const double AGGRESSIVE_AMMO_W = 0.7;
const double AGGRESSIVE_DIST_W = 0.8;
const double AGGRESSIVE_VISIBLE_W = 0.25;
const double AGGRESSIVE_GRENADES_W = 0.35;

const double CAUTIOUS_HP_W = 0.65;
const double CAUTIOUS_AMMO_W = 0.5;
const double CAUTIOUS_DIST_W = 0.66;
const double CAUTIOUS_VISIBLE_W = 0.15;
const double CAUTIOUS_GRENADES_W = 0.25;

// Tasks
const int CALL_FOR_MEDKIT = 40;
const int CALL_FOR_AMMO = 41;
const int MOVE_TO_SUPPORTER = 42;
const int FOLLOW_AGGRESSIVE_TEAMMATE = 43;
const int SEARCH_ENEMIES = 44;
const int SOLDIER_BATTLE_MODE = 45;

// Todo: may need to be more internal tasks

class Soldier :
    public Player
{
private:
    int num_bullets;
    int num_grenades;
    int soldierType;
    bool needMedkit;
    bool needAmmo;
    void FindClosestEnemyInMap(int* trow, int* tcol, vector<Player>& enemies);
    void FindEnemyToFight(int* trow, int* tcol, vector<Player>& enemies, bool visibillity_map[MSZ][MSZ],
        double* isVisible);
    void FindClosestRoom(int* trow, int* tcol, Room rooms[NUM_ROOMS]);
    void GetCloserToEnemy(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], int trow, int tcol);
    void AttackEnemy(int trow, int tcol, bool isVisible);
public:
    Soldier();
    ~Soldier();
    Soldier(int team, int id, int type);
    int getBullets() { return num_bullets; }
    int getGrenades() { return num_grenades; }
    int getSoldierType() { return soldierType; }
    void setBullets(int bullets) { num_bullets = bullets; }
    void setGrenades(int grenades) { num_grenades = grenades; }
    void setNeedMedkit(bool v) { needMedkit = v; }
    void setNeedAmmo(bool v) { needAmmo = v; }
    void CalculateTask();
    void CallForMedkit();
    void CallForAmmo();
    void RunAway(int maze[MSZ][MSZ], Room rooms[NUM_ROOMS], double security_map[MSZ][MSZ]);
    void MoveOnTowardsSupporter(int maze[MSZ][MSZ], vector <Supporter>& supporters, double security_map[MSZ][MSZ]); // Todo: change to "Player"
    void FollowAggressiveTeammate(int maze[MSZ][MSZ], vector <Soldier>& soldiers, double security_map[MSZ][MSZ]);
    void SearchTheEnemies(int maze[MSZ][MSZ], Room rooms[NUM_ROOMS], vector <Player>& enemies, double security_map[MSZ][MSZ]);
    void BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room rooms[NUM_ROOMS], vector<Player>& enemies,
        bool visibillity_map[MSZ][MSZ]);
    bool getNeedMedkit() { return needMedkit; }
    bool getNeedAmmo() { return needAmmo; }
    Bullet* ShootBullet(int trow, int tcol);
    Grenade* ThrowGrenade(int trow, int tcol);
};
