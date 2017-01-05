// TODO
#include <string.h>
#include "samurai.hpp"

// ------------------------------------------------------------
// 変数、関数の宣言 
const int homeX[2][3] = {{0, 0, 7}, {14, 14, 7}};
const int homeY[2][3] = {{0, 7, 0}, {14, 7, 14}};

// ------------------------------------------------------------
// 関数の宣言 
static char getNonBlank(istream& is); 
string readString(istream& is); 
// ------------------------------------------------------------

// ------------------------------------------------------------
// 構造体の詳細
DBEntry::DBEntry(char* buf, string dirPath) {
	// dif -> samurai.hpp
	istringstream is; // 入力準備1 cin的なものを作り出している
	is.str(string(buf)); // 入力準備2 isの中に入力を全て入れてしまう
	// string(buf) -> 1 random randomPlayer $1 のような形
	is >> id;
	name = readString(is); // ex) random
	progPath = readString(is); // ex) randomPlayer
	invocation = dirPath + readString(is); // players/$1 になる
	while (!is.eof() && isspace(is.peek())) {
		is.get();   
	}
}

Player::Player(char* nm, char* invoke, char* pause, char* resume, int side):
	autoPlay(true), name(string(nm==nullptr ? "" : nm)),
	invocation(string(invoke)),
	pause(string(pause)), resume(string(resume)), side(side) {
	}

Player::Player(DBEntry& entry, int side):
	autoPlay(false), name(entry.name), progPath(entry.progPath),
	invocation(entry.invocation), side(side) {
	}

void Player::initCommunication(char* logPath) {
	// toAI, fromAIの設定
	char cdirname[] = "/tmp/SamurAIXXXXXX";
	mkdtemp(cdirname) == 0;
	// cdirnameのXXXXXXの値を作成
	// この文がないとerrnoの値が0じゃなくなりうまく動かない
	// cdirname = /tmp/SamurAIWsgHc1 のようになる

	string dirname(cdirname);
	// char*型 を string型にして扱いやすくする

	toAIpath = dirname + "/toAI";
	mkfifo(toAIpath.c_str(), 0600);
	// toAIpath = /tmp/SamurAIWsgHc1/toAI のようになる

	fromAIpath = dirname + "/fromAI";
	mkfifo(fromAIpath.c_str(), 0600);
	// fromAIpath = /tmp/SamurAIWsgHc1/fromAI のようになる

	processId = fork();
	// 現在のプロセスを複製した子プロセスを作成
	// 子プロセスのprocessIdは0,親プロセスは子プロセスのプロセスID(10703とか)

	if (processId == 0) {
		// 子プロセスのみ処理
		freopen(toAIpath.c_str(), "r", stdin);
		freopen(fromAIpath.c_str(), "w", stdout);
		// freopen(char* filename, char* mode, FILE* stream)
		// filename -> 開くファイル名
		// mode -> fopenと同じような使い方 "r", "w"など
		// stream -> stdin, stdout, stderr など

		string command(invocation); // 初期化 invocaton = "players/$1"
		for (size_t pos = command.find("$1");
				pos != string::npos;
				pos = command.find("$1")) {
			// ここまでfor文の条件文
			// size_t -> unsigned long int (long unsigned int) の typedef 
			// (unsigned long のこともある)
			command.replace(pos, 2, progPath);
			// <string>before.replace(int start; int numChar; string after);
			// beforeのstart文字目からnumChar文字をafterで置き換える
			// command = "players/RandomPlayer" みたいな感じになる
		}

		string commandArgs(command);
		int nArgs = 1;
		char* args[nArgs+1];
		const char* str = commandArgs.c_str();
		args[nArgs-1] = new char[strlen(str)+1];
		strcpy(args[nArgs-1], str);
		args[nArgs] = nullptr;
		execvp(commandArgs.c_str(), args); // TODO
		exit(-1);
	} else if (processId != -1){
		// 親プロセスのみ
		ofstream* out = new ofstream(toAIpath); // 出力先のファイル名を指定
		toAI = out;
		fromAI = new ifstream(fromAIpath);
	}
}

void Player::finalize() {
	remove(toAIpath.c_str());
	remove(fromAIpath.c_str());
	remove(dirPath.c_str());
}
// ------------------------------------------------------------
static char getNonBlank(istream& is) {
	// 文字列読み取り(副) -> 空白を取り除き、1文字目を返す
	int ch;
	do {
		ch = is.get();
	} while (isblank(ch)); 
	// 空白ならループする。intblank(<int>)
	return ch;
	// players.txtなら上から順に r, g, ", t, " が返る
}

string readString(istream& is) {
	// 文字列読み取り(主) -> 文字列を返す(空白から空白まで or "<string>"の中)
	int ch = getNonBlank(is);
	string s;
	if (ch == '"') {
		ch = is.get();
		while (ch != '"') {
			s += ch;
			ch = is.get();
		}
	} else {
		while (!is.eof() && !isblank(ch)) {
			// eof -> 入力ファイルが終端に到達しているかどうか
			// isblank(<int>) -> 引数が空白かどうか
			s += ch;
			ch = is.get();
		}
	}
	return s;
}

// string の勉強したやつ(必要ない場所だったので消した)
// #include<string>
// std::string::c_str() 
// -> string型をchar*型に直す
// std::string::find_first_of()
// -> ()内の文字が初めて出てくる場所を返す(例の場合は最後まで)
// std::string::npos = -1 (size_t型)
// std::string::substr(size_t first)
// -> first番目から全部を抜き出した文字列を返す
// std::string::substr(size_t first, sizt_t second)
// -> first番目からsecond個抜き出した文字列
// -> 今回はcommandではじめから空白が出てくる前までの文字列を返す

// vector の勉強したやつ(必要ない場所だったので消した)
// #include<vector>
// std::vector::push_back(element)
// -> vectorの最後に要素を挿入する

