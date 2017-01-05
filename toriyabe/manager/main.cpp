#include "samurai.hpp"

// ------------------------------------------------------------
// 変数の宣言 
GameLog gameLog;
GameState gameState;
BattleField battleField;
SamuraiState samuraiStates[2][6];
int turn;
Player* firstPlayer;
Player* secondPlayer;

NullStream nullStream;

char* dbPath;

char* commLogPath = nullptr;
// playerのstatusを決めるために使用(initCommunication, initialHandShake)

static int firstPlayerId = -1;
static int secondPlayerId = -1;
int periods = 16;
string gameName = "";

static bool dbUsed;
static char* invocation[2];
static char* pauseCommand[2];
static char* resumeCommand[2];
static char* playerName[2] = {nullptr, nullptr};

string playerDirPath;
map <int, DBEntry> playerDB;
// ------------------------------------------------------------
// 関数の宣言 
static int numericalArg(char* s); 
static void processCommandLine(int argc, char* argv[]); 
static void readPlayerDB(); 
// ------------------------------------------------------------

int main(int argc, char* argv[]) {
	turn = -1;

	processCommandLine(argc, argv);
	readPlayerDB();
	firstPlayer = new Player(playerDB[firstPlayerId], 0);
	secondPlayer = new Player(playerDB[secondPlayerId], 1);
	firstPlayer->initCommunication(commLogPath);
	firstPlayer->initialHandShake(); // TODO
	secondPlayer->initCommunication(commLogPath);
	secondPlayer->initialHandShake(); // TODO
	gameLog.init(gameName, 6*periods);
	// TODO 以降
	gameState.init();
	turn = 0;
	gameState.dumpState();
	for (int p = 0; p != periods; p++) {
		for (int a = 0; a != 2; a++) {
			for (int w = 0; w != 3; w++) {
				gameState.samuraiStates[a][w].alreadyPlayed = false;
			}
		}
		for (int s = 0; s != 3; s++) {
			firstPlayer->sendTurnInfo();
			firstPlayer->receiveActionCommands();
			gameState.dumpState();
			turn++;
			secondPlayer->sendTurnInfo();
			secondPlayer->receiveActionCommands();
			gameState.dumpState();
			turn++;
		}
	}
	for (int a = 0; a != 2; a++) {
		firstPlayer->finalize();
		secondPlayer->finalize();
	}
	gameLog.writeOut(cout);
	// 出力部分
	return 0;
}

static int numericalArg(char* s) {
	// char型の配列からなる文字列をint型の数字に直す。(一対一対応になっている)
	int v = 0;
	char* p = s; // 引数sの中身を動かしたくない
	while (isdigit(*p)) { 
		// isdigit は10進数かどうかを調べる
		// while (*p != '\0') // こっちの方がわかりやすいと思った

		v += 10*v + *p - '0'; 
		// pとvが同じ値で対応しているわけではないが一対一対応ではある(p >= 10で値が異なる) 
		// p = 43 のとき v = (10*0 + 4) + (10*(10*0 + 4) + 3) = 4 + 43 = 47

		// char型をint型に直すと値が変わるため、(int)('0') = 48 を利用している。
		// ('0'はchar型(文字)としての0、文字0のアスキーコードが48)
		// ex) *p = 9とする
		//     (int)('9') = 57 より 
		//     (int)('9') - (int)('0') = 57 - 48 = 9

		p++;
		// ポインタを1つすすめることで桁を一つ下にさげる
	}
	return v;
}


static void processCommandLine(int argc, char* argv[]) {
	// 実行時のコマンドライン引数からplayerId(playerタイプの通し番号)を求める
	firstPlayerId = numericalArg(argv[1]);
	secondPlayerId = numericalArg(argv[2]);
}

static void readPlayerDB() {
	// player/player.txt(playerタイプについての宣言を記述したファイル)
	// からデータを読みだし、構造体の各要素と対応させる
	static const int DBLINESIZEMAX = 1024;
	char lineBuf[DBLINESIZEMAX+1];
	char* dbStr = getenv("SAMURAIPLAYERSDB"); // 中はヌル
	string dbPath = string("players/players.txt");
	ifstream dbs(dbPath); // 読み込みファイルの設定
	dbs.getline(lineBuf, DBLINESIZEMAX); // ファイルから読み込み
	playerDirPath = string(lineBuf);
	if (playerDirPath.back() != '/') {
		playerDirPath += '/'; 
		// 文字列playerDirPathの最後に'/'をいれる。
	}
	while (!dbs.eof()) {
		// eof -> 入力ファイルが終端に到達しているかどうか
		dbs.getline(lineBuf, DBLINESIZEMAX);
		if (lineBuf[0] != '\0') {
			// map<int, DBEntry> でmap[int] = DBEntryの形でアクセスできるようにする
			// DBEntry型でのアクセスより簡単だから。

			DBEntry entry = DBEntry(lineBuf, playerDirPath); 
			// コンストラクタ(引数あり)の呼び出し
			playerDB[entry.id] = entry;
		}
	}
}

