#include "players.hpp"
#include <list>
#include <utility>

double territoryMerits = 1;
double selfTerritoryMerits = 0.3;
double hurtingMerits = 0.1;
double hidingMerits = 0.01;

list<int> bestPlay;
list<int> currentPlay;
double bestMerits;
SamuraiState* bestSamurai;

static const int COSTS[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1}; 

struct PlanningPlayer: Player {

	// bestSamurai, bestPlay を更新する
	void plan(GameInfo& info, SamuraiState& samurai, int budget, double merits) {
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
					//*debug << "New best ";
				} else {
					//*debug << "Not best ";
				}
				//*debug << newMerits << " by " << samurai.weapon << ':';
				//for (int a: currentPlay) *debug << " " << a;
				//*debug << '@' << 7-budget+COSTS[action] << endl;
				plan(info, samurai, budget-COSTS[action], newMerits);
				undo.apply();
				currentPlay.pop_back();
			}
		}
	}

	void firstAx(SamuraiState* samurai) {
		bestSamurai = samurai;
		bestPlay.clear();
		bestPlay.push_back(7);
		bestPlay.push_back(2);
		*debug << "hogehoge" << currentTurn << " " << samurai->weapon << endl;
	}

	void secondAx(SamuraiState* samurai) {
		bestSamurai = samurai;
		bestPlay.clear();
		bestPlay.push_back(7);
		bestPlay.push_back(4);
		*debug << "hogehoge" << currentTurn << " " << samurai->weapon << endl;
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
				*debug << "planed" << endl;
				// test good early ax
				if (currentTurn <= 6 and samurai->weapon == 2) firstAx(samurai);
				if (currentTurn <= 12 and samurai->weapon == 2) secondAx(samurai);
				// test end
			}
			cout << bestSamurai->weapon;
			*debug << "debug : ";
			*debug << bestSamurai->weapon;
			for (int action: bestPlay) {
				cout << ' ' << action;
				*debug << ' ' << action;
			}
		} else {
			SamuraiState* samurai = underRecovery.front();
			cout << samurai->weapon;
		}
		cout << " 0" << endl;
		*debug << " 0" << endl;
		cout.flush();
		debug->flush();
	}
};

Player* player = new PlanningPlayer();
