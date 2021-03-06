#include "players.hpp"
#include <list>
#include <utility>

// ------------------------------------------------------------
// ここの値を調整する
double territoryMerits = 1;
double hurtingMerits = 0.1;
double hidingMerits = 0.1;
// ------------------------------------------------------------

list<int> bestPlay;
list<int> currentPlay;
double bestMerits;
SamuraiState* bestSamurai;

static const int COSTS[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1}; 

struct PlanningPlayer: Player {
    void plan(GameInfo& info, SamuraiState&samurai, int budget, double merits) {
        for (int action = 1; action != 10; action++) {
            if (COSTS[action] <= budget && info.isValid(samurai, action )) {
                currentPlay.push_back(action);
                Undo undo;
                int territory, injury, hiding;
                info.tryAction(samurai, action, undo, territory, injury, hiding);
                double newMerits = merits
                    + territoryMerits*territory
                    + hurtingMerits*injury
                    + hidingMerits*hiding;
                if (newMerits > bestMerits) {
                    bestMerits = newMerits;
                    bestPlay = currentPlay;
                    bestSamurai = &samurai;
                } else {
                }
                plan(info, samurai, budget-COSTS[action], newMerits);
                undo.apply();
                currentPlay.pop_back();
            }
        }
    }

    void play(GameInfo& info) {
        bestMerits = -1;
        bestPlay.clear();
        currentPlay.clear();
        list<SamuraiState*> playable;
        list<SamuraiState*> underRecovery;
        for (SamuraiState& samurai: samuraiStates[0]) {
            if (samurai.done == 0) {
                if (samurai.recovery == 0) {
                    playable.push_back(&samurai);
                } else {
                    underRecovery.push_back(&samurai);
                }
            }
        }
        if (!playable.empty()) {
            for (SamuraiState* samurai: playable) {
                plan(info, *samurai, 7, 0);
            }
            cout << bestSamurai->weapon;
            for (int action: bestPlay) {
                cout << ' ' << action;
            }
        } else {
            SamuraiState* samurai = underRecovery.front();
            cout << samurai->weapon;
        }
        cout << " 0" << endl;
        cout.flush();
    }
};

Player* player = new PlanningPlayer();
