// TODO
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <cctype>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#include "log.hpp"
#include "teestream.hpp"

// ------------------------------------------------------------
// 変数の宣言 
const int initialTimeLimit = 10000;
const int responseTimeLimit = 200;
const int fieldWidth = 15;
const int fieldHeight = 15;
const int recoveryTurns = 18;
const int turnBudget = 7;
const int visibleDistance = 5;

extern const int homeX[2][3];
extern const int homeY[2][3];

extern int periods;
extern int turn;

struct Coordinates; 
struct CoordHash; 
struct FieldMap;
struct Section;
struct BattleField; 
struct DBEntry; 
struct Player;
struct SamuraiState;
struct GameState;

extern ostream& operator<<(ostream& os, Section& s);
extern BattleField battleField;

static const char* DIRECTIONS[] =
{ "south", "east", "north", "west" };

extern map<int, DBEntry> playerDB;

extern Player* firstPlayer;
extern Player* secondPlayer;

extern string playerDirPath;

static const char* WEAPONS[] =
{ "spear", "swords", "battleaxe" };

extern SamuraiState samuraiState;

extern GameState gameState; // def main.cpp

ostream& operator<<(ostream& os, const Coordinates& coord);

void receiveFromAI(istream& is, GameState& gameState,
		const int side, const int weapon,
		ostream& log, int turn);

// ------------------------------------------------------------
// 構造体の詳細
struct Coordinates {
	const int x;
	const int y;
	Coordinates(int x, int y); // def battlefield.cpp
	Coordinates north() const; // def battlefield.cpp
	Coordinates south() const; // def battlefield.cpp
	Coordinates east() const; // def battlefield.cpp
	Coordinates west() const; // def battlefield.cpp
	Coordinates operator+(const Coordinates c) const; // def battlefield.cpp
	Coordinates rotate(const int direction) const; // def battlefield.cpp
	string toString() const; // def battlefield.cpp
	bool operator==(const Coordinates another) const; // def battlefield.cpp
};

struct CoordHash {
	size_t operator()(const Coordinates& c) const;
};

// TODO
struct FieldMap: unordered_map <Coordinates, Section*, CoordHash> {
	Section* locate(Coordinates p);
};

// TODO
struct Section {
	const Coordinates coords;
	int state;		      // -1: free; s>=0: occupied by samurai s
	int homeOf;		      // Home of which samurai; -1 if of none
	int population;
	SamuraiState* apparent;
	Section *neighbors[4];
	Section(Coordinates coords);// TODO
	void setNeighbors(FieldMap& map);// TODO
	void leave(SamuraiState* s);// TODO
	void arrive(SamuraiState* s);// TODO
	void occupy(int sid);// TODO
	string toString() const;
};

// TODO
struct BattleField {
	FieldMap map;
	BattleField(); // def battlefield.cpp
	Section* section(int x, int y);// def game.cpp
	void occupy(SamuraiState& samurai, int direction, Section& pos);// TODO
};

struct DBEntry {
	int id;
	int side;
	string name;
	string progPath;
	string invocation;
	DBEntry() {};
	DBEntry(char *buf, string dirPath);
};

// TODO
struct Player {
	bool autoPlay;
	int side;
	string name;
	string progPath;
	string invocation;
	string pause;
	string resume;
	pid_t processId;
	string dirPath;
	string toAIpath;
	string fromAIpath;
	ostream* toAI;
	istream* fromAI;
	int response;
	bool alive;
	bool done;
	void initCommunication(char* logPath);
	void initialHandShake();// TODO
	void receiveAcknowledgement();// TODO
	void sendTurnInfo();// TODO
	void receiveActionCommands();// TODO
	void finalize();
	Player(DBEntry& entry, int side);
	Player(char* name, char* invoke, char* pause, char* resume, int side);
};

// TODO
struct SamuraiState {
	int side;
	int weapon;
	Section* position;
	Section* home;
	int curedAt;
	bool alive;
	bool hidden;
	bool alreadyPlayed;
	string dirname;
	queue<int> actions;

	void init(BattleField& field, int side, int weapon);// TODO
	bool move(int direction);// TODO
	bool occupy(int direction);// TODO
	bool hideAppear();// TODO
	void house(BattleField& field);// TODO
	void injure(BattleField& field);// TODO
};

// TODO
struct GameState {
	SamuraiState samuraiStates[2][3];
	GameState() {}; // コンストラクタ(特に何もしない)
	void init();// TODO
	void dumpState();// TODO
};
// ------------------------------------------------------------

