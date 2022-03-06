#pragma once
#include "Player.h"
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
const int GET_CLOSER_TO_ENEMY = 46;
const int RUN_AWAY = 47;
const int SHOOT_BULLET = 48;
const int THROW_GRENADE = 49;

class Supporter;

class Soldier :
    public Player
{
private:
    int num_bullets;
    int num_grenades;
    int soldierType;
    int enemy_row, enemy_col;
    bool needMedkit;
    bool needAmmo;
    void FindClosestEnemyInMap(int* trow, int* tcol, vector<Player*>& enemies);
    void FindEnemyToFight(int* trow, int* tcol, vector<Player*>& enemies, bool visibillity_map[MSZ][MSZ],
        double* isVisible);
    void FindClosestRoom(int* trow, int* tcol, Room* rooms[NUM_ROOMS]);
    void AttackEnemy(bool isVisible);
    void UpdateNextSoldierStep(int maze[MSZ][MSZ], Cell* next);
public:
    Soldier();
    ~Soldier();
    Soldier(int team, int id, int type);
    int getBullets() { return num_bullets; }
    int getGrenades() { return num_grenades; }
    int getSoldierType() { return soldierType; }
    int getEnemyRow() { return enemy_row; }
    int getEnemyCol() { return enemy_col; }
    bool getNeedMedkit() { return needMedkit; }
    bool getNeedAmmo() { return needAmmo; } void setBullets(int bullets) { num_bullets = bullets; }
    void setGrenades(int grenades) { num_grenades = grenades; }
    void setNeedMedkit(bool v) { needMedkit = v; }
    void setNeedAmmo(bool v) { needAmmo = v; }
    void setSoldierType(int type) { soldierType = type; }
    void CalculateTask(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector<Player*>& enemies,
        bool visibillity_map[MSZ][MSZ], Supporter* sp);
    void CallForMedkit();
    void CallForAmmo();
    void RunAway(int maze[MSZ][MSZ], Room* rooms[NUM_ROOMS], double security_map[MSZ][MSZ]);
    void MoveOnTowardsSupporter(int maze[MSZ][MSZ], Supporter* sp, double security_map[MSZ][MSZ]);
    void FollowAggressiveTeammate(int maze[MSZ][MSZ], vector <Soldier*>& soldiers, double security_map[MSZ][MSZ]);
    void SearchTheEnemies(int maze[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector <Player*>& enemies, double security_map[MSZ][MSZ]);
    void BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], Room* rooms[NUM_ROOMS], vector<Player*>& enemies,
        bool visibillity_map[MSZ][MSZ]);
    void GetCloserToEnemy(int maze[MSZ][MSZ], double security_map[MSZ][MSZ]);
    Bullet* ShootBullet();
    Grenade* ThrowGrenade();
};
