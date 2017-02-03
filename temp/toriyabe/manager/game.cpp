// TODO
#include "samurai.hpp"
#include <thread>
#include <signal.h>
#include <wait.h>

void SamuraiState::init(BattleField& field, int sd, int wp) {
    // 一人の侍についてステータスを決定する
	side = sd;
	weapon = wp;
	position = field.section(homeX[sd][wp], homeY[sd][wp]);
	position->arrive(this);
	position->homeOf = 3*side+weapon;
	home = position;
	home->state = 3*side + weapon;
	alive = true;
	curedAt = -1;
	hidden = false;
	alreadyPlayed = false;
}

// TODO
bool SamuraiState::move(int direction) {
	Section* p = position->neighbors[direction];
	if (p == 0) {
		return false;
	}
	for (int a = 0; a != 2; a++) {
		for (int w = 0; w != 3; w++) {
			SamuraiState& ss = gameState.samuraiStates[a][w];
			if (&ss != this && p == ss.home) {
				return false;
			}
		}
	}
	if (hidden) {
		if (p->state < 0 || side != p->state/3) {
			return false;
		}
	}
	position->leave(this);
	position = p;
	position->arrive(this);
	return true;
}

// TODO
bool SamuraiState::occupy(int direction) {
	battleField.occupy(*this, direction, *position);
	return true;
}

// TODO
bool SamuraiState::hideAppear() {
	if (hidden) {
		if (position->apparent != nullptr) {
			return false;
		}
		hidden = false;
		position->apparent = this;
	} else {
		if (position->state < 0 || position->state/3 != side) {
			return false;
		}
		hidden = true;
		position->apparent = nullptr;
	}
	return true;
}

// TODO
void SamuraiState::house(BattleField& field) {
	position->leave(this);
	hidden = false;
	home->arrive(this);
	position = home;
}

// TODO
void SamuraiState::injure(BattleField& field) {
	house(field);
	curedAt = turn + recoveryTurns;
}

// TODO
void readResponse(Player& player) {
	*player.fromAI >> player.response;
	player.done = true;
}

// TODO -> 理解不能 
void Player::initialHandShake() {
	*toAI << side << endl; // バッファへ内容を書き込む
	toAI->flush(); // バッファの内容を外部ファイルに転送する
	response = -1;
	done = false;
	thread* readerThread = new thread(readResponse, ref(*this));
	// threadの初期化,thisはこの関数を呼び出しているPlayerのこと

	chrono::system_clock::time_point start = chrono::system_clock::now();
	chrono::system_clock::time_point till = 
		start + chrono::milliseconds(initialTimeLimit);
	chrono::system_clock::time_point now;
	// chronoは時間を測るためのライブラリ
	// initialTimeLimit = 10000 // def samurai.hpp

	while (!done) {
		// 親スレッドのみ
		this_thread::sleep_for(chrono::milliseconds(1));
		// 1ミリ秒だけこのスレッド(親スレッド)をスリープさせる(タイミング調整のためだと思われる)
		now = chrono::system_clock::now();
		if (now >= till) break;
		// 時間がかかりすぎかを確かめる
	}

	// kill関数はプロセスにシグナルを送る
	// signal関数
	// -> SIGSTOP ~ プロセスの一時停止
	// -> SIGCONT ~ プロセスの再開
	// -> SIGKILL ~ プロセスの強制終了
    
	kill(processId, SIGSTOP);
	alive = done;
	if (!alive) {
		cerr << "Time out at initiation" << endl; //abcde
		readerThread->detach();
        // std::thread::detach() -> スレッドの管理を手放す
		if (!autoPlay) {
			kill(processId, SIGKILL);
			int status;
			wait(&status);
		}
	} else {
		readerThread->join();
		delete readerThread;
		if (response != 0) {
			cerr << "Wrong response at initiation" << endl;
			alive = false;
		}
	}
}

// TODO
void GameState::init() {
	for (int side = 0; side != 2; side++) {
		for (unsigned int weapon = 0; weapon != 3; weapon++) {
			SamuraiState& ss = samuraiStates[side][weapon];
			ss.init(battleField, side, weapon);
		}
	}
}

// TODO
void Player::sendTurnInfo() {
	if (alive) {
		// Send turn number
		*toAI << turn << endl;
		// Calculate Visibility
		bool visible[fieldWidth*fieldHeight] = {};
		for (int weapon = 0; weapon != 3; weapon++) {
			const Section& s = *gameState.samuraiStates[side][weapon].position;
			int x0 = s.coords.x;
			int y0 = s.coords.y;
			for (int dy = -visibleDistance; dy <= visibleDistance; dy++) {
				int y = y0+dy;
				if (0 <= y && y < fieldHeight) {
					int v1 = visibleDistance-abs(dy);
					for (int dx = -v1; dx <= v1; dx++) {
						int x = x0+dx;
						if (0 <= x && x < fieldWidth) {
							visible[y*fieldWidth+x] = true;
						}
					}
				}
			}
		}
		// Send samurai states
		for (int w = 0; w != 3; w++) {
			// Friend samurai
			SamuraiState& ss = gameState.samuraiStates[side][w];
			int x = ss.position->coords.x;
			int y = ss.position->coords.y;
			*toAI << x << ' ' << y << ' ' 
				<< (ss.alreadyPlayed ? 1  : 0) << ' '
				<< (ss.hidden ? 1 : 0) << ' '
				<< (ss.curedAt > turn ? ss.curedAt-turn : 0) 
				<< endl;
		}
		for (int w = 0; w != 3; w++) {
			// Enemy samurai
			SamuraiState& ss = gameState.samuraiStates[1-side][w];
			int x = ss.position->coords.x;
			int y = ss.position->coords.y;
			if (visible[y*fieldWidth+x] && !ss.hidden) {
				// Visible
				*toAI << x << ' ' << y << ' ' 
					<< (ss.alreadyPlayed ? 1  : 0) << ' '
					<< (ss.hidden ? 1 : 0);
			} else {
				// Invisible
				*toAI << -1 << ' ' << -1 << ' '
					<<  (ss.alreadyPlayed ? 1 : 0) << ' '
					<< 1;
			}
			*toAI << ' '
				<< (ss.curedAt > turn ? ss.curedAt-turn : 0) 
				<< endl;
		}
		// Send field states
		for (int y = 0; y != fieldHeight; y++) {
			for (int x = 0; x != fieldWidth; x++) {
				Section& sect =
					*battleField.map.locate(Coordinates(x, y));
				if (visible[y*fieldWidth+x]) {
					int sectState =
						(sect.state == -1 ? 8 :
						 (side == 0 ? sect.state :
						  (sect.state >= 3 ? sect.state -3 :
						   sect.state + 3)));
					*toAI << ' ' << sectState;
				} else if (sect.homeOf >= 0) {
					*toAI << ' '
						<< (side == 0 ? sect.homeOf :
								sect.homeOf >= 3 ? sect.homeOf-3 :
								sect.homeOf+3);
				} else {
					*toAI << ' ' << 9;
				}
			}
			*toAI << endl;
		}
		toAI->flush();
	}
}

// TODO
void readOrder(Player& player, int& weapon, list <int>& order) {
	int w;
	*player.fromAI >> w;
	if (!player.alive) return;
	weapon = w;
	while (true) {
		int action;
		*player.fromAI >> action;
		if (!player.alive) return;
		if (!player.fromAI->good()) {
			player.alive = false;
			break;
		}
		if (action == 0) break;
		order.push_back(action);
	}
	player.done = true;
}

// TODO
void Player::receiveActionCommands() {
	list<int> order;
	list<int> orderSave;
	int weapon;
	if (alive) {
		kill(processId, SIGCONT);
		done = false;
		thread* readerThread =
			new thread(readOrder, ref(*this), ref(weapon), ref(order));
		// threadは並列処理に使う
		chrono::system_clock::time_point start = chrono::system_clock::now();
		chrono::system_clock::time_point till =
			start + chrono::milliseconds(responseTimeLimit);
		chrono::system_clock::time_point now;
		while (!done) {
        // infinite loop
			this_thread::sleep_for(chrono::milliseconds(1));
			now = chrono::system_clock::now();
			if (now >= till) break;
		}
		int responseTime = (now - start).count()/1000000;
		if (!done) {
        // infinite loop
			readerThread->detach();
			if (!autoPlay) {
				kill(processId, SIGKILL);
				int status;
				wait(&status);
			}
			goto DEAD;
		}
		kill(processId, SIGSTOP);
		readerThread->join();
		delete readerThread;
		orderSave = order;
		if (weapon < 0 || weapon > 2) {
			goto FINISH;
		}
		{
			int budget = turnBudget;
			SamuraiState& ss = gameState.samuraiStates[side][weapon];
			if (!order.empty() && ss.curedAt > turn) {
				goto FINISH;
			}
			if (!order.empty() && ss.alreadyPlayed) {
				goto FINISH;
			}
			ss.alreadyPlayed = true;
			for (int action: order) {
				if (action <= 0 || action > 9) {
					break;
				} else {
					static int cost[] = {0, 4, 4, 4, 4, 2, 2, 2, 2, 1}; 
					budget -= cost[action];
					if (budget < 0) {
						break;
					}
					if (action <= 4) {
						if (!ss.occupy(action-1)) goto FINISH;
					} else if (action <= 8) {
						if (!ss.move(action-5)) goto FINISH;
					} else if (action == 9) {
						if (!ss.hideAppear()) goto FINISH;
					}
				}
			}
		}
FINISH:
		int territories[6] = { 0 };
		for (int y = 0; y != fieldHeight; y++) {
			for (int x = 0; x != fieldWidth; x++) {
				Section& sect = *battleField.map.at(Coordinates(x, y));
				if (sect.state >= 0) {
					territories[sect.state] += 1;
				}
			}
		}
		gameLog.addTurnPlay(weapon, orderSave, responseTime, territories);
		return;
	}
DEAD:
	int territories[6] = { 0 };
	for (int y = 0; y != fieldHeight; y++) {
		for (int x = 0; x != fieldWidth; x++) {
			Section& sect = *battleField.map.at(Coordinates(x, y));
			if (sect.state >= 0) {
				territories[sect.state] += 1;
			}
		}
	}
	gameLog.addTurnPlay(-1, orderSave, 0, territories);
	return;
}

// TODO
void GameState::dumpState() {
	int territories[10] = {0};
	for (int y = 0; y != fieldHeight; y++) {
		for (int x = 0; x != fieldWidth; x++) {
			Section& sect = *battleField.map.at(Coordinates(x, y));
			if (sect.state >= 0) {
				territories[sect.state] += 1;
			}
		}
	}
	for (int a = 0; a != 2; a++) {
		for (int w = 0; w != 3; w++) {
			SamuraiState& ss = samuraiStates[a][w];
		}
	}
}
