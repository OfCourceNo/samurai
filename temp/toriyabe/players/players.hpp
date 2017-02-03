// TODO
#include <iostream>
#include <cstring>
#include <list>

using namespace std;

// ------------------------------------------------------------
// 変数の宣言 
const int width = 15; // battlefieldの横
const int height = 15; // battlefieldの縦
const int recoveryTurns = 18; // 侍が倒れてから生き返るまでのターン

extern int field[width][height]; // def players.cpp
extern int playOrder; // def players.cpp, gameInfo::gameInfo
extern int totalTurns; // def players.cpp
extern int currentTurn; // def players.cpp

struct SamuraiState; 
extern SamuraiState samuraiStates[2][3]; // def manager/main.cpp
struct FieldUndo; 
struct SamuraiUndo; 
struct Undo; 
struct GameInfo; 
struct Player; 
extern Player* player; // def selfPlayer.cpp
// ------------------------------------------------------------

// ------------------------------------------------------------
// 関数の宣言 
void rotate(int direction, int x0, int y0, int& x, int& y); 
// def players.cpp
// ------------------------------------------------------------

// ------------------------------------------------------------
// 構造体の詳細
struct SamuraiState {
	int homeX; // home位置x
	int homeY; // home位置y
	int weapon; // 所持武器 0:槍, 1:刀, 2:鉞
	int x; // 現在位置x
    int y; // 現在位置y
	int done; // 行動済みか 1:行動済み
	int hidden; // 隠伏状態 1:隠れている
	int recovery; // 療治ターン数

	void initSamuraiState(int a, int w);
    // x, y, done, hidden, recoveryの読み込み
	bool isValidAction(const int action) const;
    // 選択した行動を取れるかどうかを返す
};


struct FieldUndo {
	int *section;
	int state;
	FieldUndo(int* sect); // コンストラクタ
    // sectionとstateに値を入れる
	void apply();
};

struct SamuraiUndo {
	SamuraiState* ss;
	int x;
	int y;
	int hidden;
	SamuraiUndo(SamuraiState* ss); // コンストラクタ
	void apply();
};

struct Undo {
	list<FieldUndo> fieldUndo;
	list<SamuraiUndo> samuraiUndo;
	void recField(int* sect);// TODO
	void recSamurai(SamuraiState* si);// TODO
	void apply();// TODO
};

struct GameInfo {
	GameInfo(); // コンストラクタ
	void readTurnInfo();
	bool isValid(SamuraiState& ss, int action) const;
	void tryAction(SamuraiState& samurai, int action, Undo& undo,
			int& territory, int& injury, int& hiding);// TODO
};

struct Player {
    // ここの詳細を別のソースコードに作成する
	virtual void play(GameInfo& info) = 0;// ずっと TODO
};
// ------------------------------------------------------------

