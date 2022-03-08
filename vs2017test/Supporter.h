#pragma once
#include "Soldier.h"

const int MAX_AMMO = 3;
const int MAX_MEDKITS = 3;

// Tasks
const int FILL_MEDKIT_STOCK = 30;
const int FILL_AMMO_STOCK = 31;
const int PROVIDE_MED_TO_URGENT = 32;
const int PROVIDE_AMMO_TO_URGENT = 33;
const int USE_MEDKIT = 34;
const int FOLLOW_TEAMMATES = 35;
const int SUPPORTER_BATTLE_MODE = 36;

class Supporter :
    public Player
{
private:
    int num_ammo;
    int num_medkits;
    int soldierToProvide;
    int itemProvided;
    bool isLastSurvivor;
    bool CheckIfSoldierNeedHP(vector<Soldier*>& teammates);
    bool CheckIfSoldierNeedAmmo(vector<Soldier*>& teammates);
    Soldier* GetProvidedSoldier(vector<Soldier*>& teammates);
    void UpdateNextSupporterStep(int maze[MSZ][MSZ], Cell* next);
public:
    Supporter();
    ~Supporter();
    Supporter(int team, int id);
    int getAmmo() { return num_ammo; }
    int getMedkits() { return num_medkits; }
    int getSoldierProvided() { return soldierToProvide; }
    void setIsLastSurvivor(bool isLast) { isLastSurvivor = isLast; }
    void setAmmo(int ammo) { num_ammo = ammo; }
    void setMedkits(int meds) { num_medkits = meds; }
    void setSoldierProvided(int id) { soldierToProvide = id; }
    void CalculateTask(vector<Player*>& enemies, vector<Soldier*>& teammates);
    void FollowTeammates(int maze[MSZ][MSZ], vector<Soldier*>& soldiers, double security_map[MSZ][MSZ]);
    void UseMedkit();
    void FillMedkitStock(int maze[MSZ][MSZ], Room* med_rooms[NUM_MED_ROOMS], double security_map[MSZ][MSZ]);
    void FillAmmoStock(int maze[MSZ][MSZ], Room* ammo_rooms[NUM_AMMO_ROOMS], double security_map[MSZ][MSZ]);
    void ProvideAmmoToSoldier(int maze[MSZ][MSZ], Soldier* sd, double security_map[MSZ][MSZ]);
    void ProvideMedkitToSoldier(int maze[MSZ][MSZ], Soldier* sd, double security_map[MSZ][MSZ]);
    void BattleMode(int maze[MSZ][MSZ], double security_map[MSZ][MSZ], vector<Soldier*>& teammates);
};

