// 全体をauto indentしないほうがよい　
// TODO
#include "players.hpp"
#include <unistd.h>
#include <cstdlib>
#include <fstream>

// ------------------------------------------------------------
// 変数の宣言 
int playOrder; // プレイヤー判定
int totalTurns = 96; // 総ターン数
int currentTurn; // 現在のターン数
int field[15][15];  // battlefield

SamuraiState samuraiStates[2][3];
// 各プレイヤー3人ずつの侍の情報を保存する
// 槍、刀、鉞の順

static const int homes[2][3][2] = {
  {{ 0, 0}, { 0, 7}, {7, 0}},
  {{14,14}, {14, 7}, {7,14}}
};
// 各侍の初期位置を表す

static const int osize[3] = {4, 5, 7};
// 各武器の攻撃範囲

// ------------------------------------------------------------
// 現在位置から見た攻撃位置の相対位置
static const int ox[3][7] = {
  { 0, 0, 0, 0},
  { 0, 0, 1, 1, 2},
  {-1,-1,-1, 0, 1, 1, 1}};

static const int oy[3][7] = {
  { 1, 2, 3, 4},
  { 1, 2, 0, 1, 0},
  {-1, 0, 1, 1, 1, 0,-1}}; 
// ------------------------------------------------------------

// ------------------------------------------------------------
// 関数の宣言 
char getChar(); 
int getInt();
void rotate(int direction, int x0, int y0, int& x, int& y); // TODO
// ------------------------------------------------------------

// ------------------------------------------------------------
// 構造体の詳細
void SamuraiState::initSamuraiState(int a, int w) {
  weapon = w; // w = 0, 1, 2でそれぞれの武器
  int s = (playOrder == a ? 0 : 1); // どちらのプレイヤーの兵士かを判別する
  homeX = homes[s][w][0];
  homeY = homes[s][w][1];
  x = getInt(); // 情報の読み取り
  y = getInt(); // 情報の読み取り
  done = getInt(); // 情報の読み取り
  hidden = getInt(); // 情報の読み取り
  recovery = getInt(); // 情報の読み取り
}

GameInfo::GameInfo() {
  playOrder = getInt(); // どちらのプレイヤーかを判別する
  // int playOrder -> def here 
  cout << 0 << endl;
}

void GameInfo::readTurnInfo() {
  currentTurn = getInt();
  for (int a = 0; a != 2; a++) {
    for (int w = 0; w != 3; w++) {
      samuraiStates[a][w].initSamuraiState(a, w);
      // それぞれのプレイヤーのすべての兵士の状態を読み取る
      // aはプレイヤー, wは武器種を表す
    }
  }
  for (int y = 0; y != height; y++) {
    for (int x = 0; x != width; x++) {
      field[x][y] = getInt(); // fieldの情報をよみとる
    }
  }
}

bool GameInfo::isValid(SamuraiState& ss, const int action) const {
  // 下の関数呼び出すだけ
  return ss.isValidAction(action);
}

// auto indentしないほうがよい　
bool SamuraiState::isValidAction(const int action) const {
    //　選択した行動ができるかどうかを返す
    if (recovery != 0) return false;
    // 侍が治療中なら何もできない

    switch (action) {
        case 1: case 2: case 3: case 4:
            // Occupation
            // 潜伏している時は占領できない
            return hidden == 0;
        case 5: case 6: case 7: case 8: {
            // 現在位置を動かさずに行動できるか調べるために定義
            int nx = x;
            int ny = y;

            switch (action) {
                case 5: ny += 1; break;
                case 6: nx += 1; break;
                case 7: ny -= 1; break;
                case 8: nx -= 1; break;
            }

            // fieldの外に出ない
            if (nx < 0 || width <= nx || ny < 0 || height <= ny) return false;

            // 潜伏している時に相手のテリトリーに入る(誰のテリトリーでもない場所を含む)
            if (hidden != 0 && 3 <= field[nx][ny]) return false;

            // プレイヤー番号で回す
            for (int a = 0; a != 2; a++) {
                // pythonのリストの要素で回すときのような使い方
                for (SamuraiState& ss: samuraiStates[a]) {
                    // 自分が潜伏していない時に他の潜伏していない侍と位置が被ってはいけない。
                    // 自分もしくは他のプレイヤーが潜伏している時に位置が被るのは良いと予想
	                if (hidden == 0 && nx == ss.x && ny == ss.y && ss.hidden == 0)
	                    return false;
                    // 他の侍のホームに入ってはいけない
	                if (&ss != this && nx == ss.homeX && ny == ss.homeY)
	                    return false;
                }
            }
            return true;
        }
        case 9:
            if (hidden == 0) {
            // これから潜伏する場合
                // 自分の侍(三人あわせて)が占領していない場所では潜伏できない
                if (3 <= field[x][y]) return false;
            } else {
            // 潜伏状態を解く場合
                for (int a = 0; a != 2; a++) {
	                for (SamuraiState& ss: samuraiStates[a]) {
                        // 潜伏状態を解く場所に潜伏していない他の侍がいてはならない
	                    if (&ss != this && ss.hidden == 0 && x == ss.x && y == ss.y)
	                        return false;
	                }
                }
            }
            return true;
        // それ以外なら何もせずに終わるという意味だと思われる
        default:
            exit(1);
    }
}

FieldUndo::FieldUndo(int* sect): section(sect), state(*section) {}
void FieldUndo::apply() { *section = state; }

SamuraiUndo::SamuraiUndo(SamuraiState* who): ss(who) {
// SamuraiState* ssをwhoで初期化、x,y,hiddenもwhoの中身をコピー
  x = who->x; y = who->y; hidden = who->hidden;
}

void SamuraiUndo::apply() {
// SamuraiState* ssの値を変えた際に他の要素に適用する
  ss->x = x; ss->y = y; ss->hidden = hidden;
}

// TODO
void Undo::recField(int* sect) { fieldUndo.emplace_front(sect); }
// list::emplace_front(element) -> 要素の追加

// TODO
void Undo::recSamurai(SamuraiState* ss) {
  samuraiUndo.emplace_front(ss);
}

// TODO
void Undo::apply() {
  for (FieldUndo& u: fieldUndo) u.apply();
  for (SamuraiUndo& u: samuraiUndo) u.apply();
}

// TODO
void GameInfo::tryAction(SamuraiState& samurai, int action, Undo& undo, 
			 int& territory, int& injury, int& hiding) {
  territory = injury = hiding = 0;
  int weapon = samurai.weapon;
  switch (action) {
  case 1: case 2: case 3: case 4: {
    // Occupy
    for (int k = 0; k != osize[weapon]; k++) {
      int xx, yy;
      rotate(action-1, ox[weapon][k], oy[weapon][k], xx, yy);
      xx += samurai.x; yy += samurai.y;
      if (0 <= xx && xx < width && 0 <= yy && yy < height) {
	for (int a = 0; a != 2; a++) {
	  for (SamuraiState& ss: samuraiStates[a]) {
	    if (ss.homeX == xx && ss.homeY == yy) goto ISAHOME;
	  }
	}
	{
	  int current = field[xx][yy];
	  if (current == 8) {
	    // unoccupied
	    territory += 1;
	  } else if (current >= 3) {
	    // opponent's territory
	    territory += 2;
	  }
	  for (SamuraiState& ss: samuraiStates[1]) {
	    if (ss.x == xx && ss.y == yy) {
	      undo.recSamurai(&ss);
	      ss.x = ss.homeX;
	      ss.y = ss.homeY;
	      injury++;
	      ss.hidden = 0;
	    }
	  }
	  undo.recField(&field[xx][yy]);
	}
      ISAHOME:;
      }
    }
    break;
  }
  case 5: case 6: case 7: case 8: {
    // Move
    static const int dx[] = { 0, 1, 0, -1 };
    static const int dy[] = { 1, 0, -1, 0 };
    undo.recSamurai(&samurai);
    samurai.x += dx[action-5];
    samurai.y += dy[action-5];
    break;
  }
  case 9:
    if (samurai.hidden == 0) {
      // Hide
      undo.recSamurai(&samurai);
      samurai.hidden = 1;
      hiding += 1;
      break;
    } else {
      // Expose
      undo.recSamurai(&samurai);
      samurai.hidden = 0;
      hiding -= 1;
      break;
    }
  }
}
// ------------------------------------------------------------

int main(int argc, char* argv[]) {
	GameInfo info;
	while (true) {
		info.readTurnInfo();
		player->play(info); // def eachPlayer.cpp
	}
}

// ------------------------------------------------------------

char getChar() {
	return cin.get();
}

int getInt() {
	char c;
	do { 
        c = getChar();
        // cはおそらくひと桁のはず(一応残す)
	} while (isspace(c));
	int v = 0;
	do {
		v = 10*v + c - '0';
        // アスキーコードの関係
		c = getChar();
        // 2桁以上の時はchar*とintの値は一致しないが一対一対応にはなる
	} while (!isspace(c));
	return v;
}

// TODO
void rotate(int direction, int x0, int y0, int& x, int& y) {
	switch (direction) {
		case 0: x = x0; y = y0; break;
		case 1: x = y0; y = -x0; break;
		case 2: x = -x0; y = -y0; break;
		case 3: x = -y0; y = x0; break;
	}
}

