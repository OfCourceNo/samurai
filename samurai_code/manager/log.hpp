// 構造体の宣言
struct TurnPlay;
struct GameLog; 

// extern変数、関数の宣言
extern GameLog gameLog;

// 構造体の定義
struct TurnPlay {
	vector <string> turnComments;
	int samurai;
	int responseTime;
	list <int> actions;
	int territories[6];
	TurnPlay(int samurai, list<int>& actions, int response, int territories[]);
	void writeOut(ostream& os);
};

struct GameLog {
	string gameName;
	vector <string> gameComments;
	string playerNames[2];
	int initialResponseTimes[2];
	int totalTurns;
	vector <TurnPlay> plays;
	void init(string gn, int totalTurns);
	void addTurnPlay(int samurai, list<int>& actions,
			int reponse, int territories[]);
	void writeOut(ostream& os);
};

