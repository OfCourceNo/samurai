#include "samurai.hpp"

const int homeX[2][3] = {{0, 0, 7}, {14, 14, 7}};
const int homeY[2][3] = {{0, 7, 0}, {14, 7, 14}};

static char getNonBlank(istream& is) {
  int ch;
  do {
    ch = is.get();
  } while (isblank(ch));
  return ch;
}

string readString(istream& is) {
  int ch = getNonBlank(is);
  string s;
  if (ch == '"') {
    // quoted
    ch = is.get();
    while (ch != '"') {
      if (ch == '\\') {
	ch = is.get();
      }
      s += ch;
      ch = is.get();
    }
  } else {
    while (!is.eof() && !isblank(ch)) {
      s += ch;
      ch = is.get();
    }
  }
  return s;
}

DBEntry::DBEntry(char* buf, string dirPath) {
  istringstream is;
  is.str(string(buf));
  is >> id;
  name = readString(is);
  progPath = readString(is);
  invocation = dirPath + readString(is);
  while (!is.eof() && isspace(is.peek())) is.get();
  if (!is.eof()) {
    throw ErrorReport("Junk at the end of AI DB line:\n" + string(buf));
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
  char cdirname[] = "/tmp/SamurAIXXXXXX";
  if (mkdtemp(cdirname) == 0)
    throw ErrorReport("Filed to make a temporary directory: "+errno);
  string dirname(cdirname);
  toAIpath = dirname + "/toAI";
  mkfifo(toAIpath.c_str(), 0600);
  fromAIpath = dirname + "/fromAI";
  mkfifo(fromAIpath.c_str(), 0600);
  processId = fork();
  if (processId == -1) {
    throw ErrorReport("Failed to fork an AI process: "+errno);
  } else if (processId == 0) {
    // Child process
    //   Reopen stdin and stdout
    freopen(toAIpath.c_str(), "r", stdin);
    freopen(fromAIpath.c_str(), "w", stdout);
    string command(invocation);
    if (!autoPlay) {
      for (size_t pos = command.find("$1");
	   pos != string::npos;
	   pos = command.find("$1")) {
	command.replace(pos, 2, progPath);
      }
    }
    system(command.c_str());
    exit(0);
  } else {
    ofstream* out = new ofstream(toAIpath);
    if (!out->good())
      throw ErrorReport(string("Failed to open communication: ")+ toAIpath);
    if (logPath == nullptr) {
      toAI = out;
    } else {
      string suffixedPath = string(logPath)+to_string(side);
      ofstream* toLog = new ofstream(suffixedPath);
      if (!toLog->good())
	throw ErrorReport(string("Failed to communication log: ") +
			  suffixedPath);
      toAI = new TeeStream(*out, *toLog);
    }
    fromAI = new ifstream(fromAIpath);
  }
  *dump << (side ==0 ? "First" : "Second") << "Player" << endl
	<< "  Name: " << name << endl
	<< "  Program: " << progPath << endl
	<< "  Invocation: " << invocation << endl
	<< "  Pipe directory: " << cdirname << endl;
}

void Player::finalize() {
  remove(toAIpath.c_str());
  remove(fromAIpath.c_str());
  remove(dirPath.c_str());
}

