#include "players.hpp"

struct SelfPlayer: Player {
    int attack(GameInfo& info, SamuraiState& samurai) {
        int choicedAction = 0;
        int getField = 0;
        Undo undo;
        for (int action = 1; action <= 4; action ++) {
            int territory, injury, hiding;
            info.tryAction(samurai, action, undo, territory, injury, hiding);
            if (territory >= getField) {
                choicedAction = action;
                getField = territory;
            }
        }
        return choicedAction;
    }

    int move(GameInfo& info, SamuraiState& samurai) {
        int x = samurai.x;
        int y = samurai.y;
        if (field[x][y - 1] >= 3) {
            return 7;
        } else if (field[x - 1][y] >= 3) {
            return 8;
        } else if (field[x][y + 1] >= 3) {
            return 5;
        } else if (field[x + 1][y] >= 3) {
            return 6;    
        } else {
            return 5;
        }
    }

    void play(GameInfo& info) {
        SamuraiState* samurai;
        int choicedPlayer = currentTurn % 6 / 2;
        int action;
        samurai = &samuraiStates[0][choicedPlayer];
        cout << samurai->weapon;
        if (samurai->hidden == 0) {
            action = attack(info,*samurai);  
            cout << ' ' << action;
            action += 4;
            cout << ' ' << action;
            action = 9;
            if(info.isValid(*samurai, action)) {
                cout << ' ' << action;
            }
        } else {
            action = 9;
            cout << ' ' << action;
            action = move(info,*samurai);  
            cout << ' ' << action;
            cout << ' ' << action;
            cout << ' ' << action;
        }
        cout << " 0" << endl;
        cout.flush();
    }
};

Player* player = new SelfPlayer();
