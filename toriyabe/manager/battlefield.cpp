// TODO
#include "samurai.hpp"

Coordinates::Coordinates(const int x, const int y): x(x), y(y) {}
Coordinates Coordinates::north() const { return Coordinates(x, y-1); }
Coordinates Coordinates::south() const { return Coordinates(x, y+1); }
Coordinates Coordinates::east() const { return Coordinates(x+1, y); }
Coordinates Coordinates::west() const { return Coordinates(x-1, y); }

// operator + == の設定
Coordinates Coordinates::operator+(const Coordinates c) const {
	return Coordinates(x+c.x, y+c.y);
}
bool Coordinates::operator==(const Coordinates another) const {
	return x == another.x && y == another.y;
}

size_t CoordHash::operator()(const Coordinates& c) const {
// 何かしらの演算定義
	return 1000*c.x + c.y;
}

Coordinates Coordinates::rotate(const int direction) const {
    // 回転を表す
	switch (direction) {
		case 0:
			return Coordinates(x, y);
		case 1:
			return Coordinates(y, -x);
		case 2:
			return Coordinates(-x, -y);
		case 3:
			return Coordinates(-y, x);
	}
}

string Coordinates::toString() const {
    // xとyを出力する
	ostringstream os;
	os << "(" << x << "," << y << ")";
	return os.str();
}

// TODO
Section* FieldMap::locate(Coordinates p) {
	FieldMap::const_iterator i = find(p);
	return (i == end() ? 0 : i->second);
}

// TODO
Section::Section(Coordinates coords):
	coords(coords), state(-1), population(0), apparent(0), homeOf(-1)  {
	}

// TODO
string Section::toString() const {
	return coords.toString();
}

// TODO
ostream& operator<<(ostream& os, Section& s) {
	os << s.toString();
	return os;
}

// TODO
void Section::setNeighbors(FieldMap& map) {
	FieldMap::const_iterator south, east, north, west;
	neighbors[0] = map.locate(coords.south());
	neighbors[1] = map.locate(coords.east());
	neighbors[2] = map.locate(coords.north());
	neighbors[3] = map.locate(coords.west());
}

// TODO
void Section::leave(SamuraiState* s) {
	population -= 1;
	if (!s->hidden) {
		assert(apparent == s);
		apparent = 0;
	}
}

// TODO
void Section::arrive(SamuraiState* s) {
	population += 1;
	if (!s->hidden) {
		apparent = s;
	}
}

// TODO
void Section::occupy(int sid) {
	state = sid;
}

// TODO
BattleField::BattleField() {
	for (int x = 0; x != fieldWidth; x++) {
		for (int y = 0; y != fieldHeight; y++) {
			Coordinates coords(x, y);
			map[coords] = new Section(coords);
		}
	}
	for (auto pair: map) {
		pair.second->setNeighbors(map);
	}
}

// TODO
Section* BattleField::section(int x, int y) {
// def samurai.cpp
	return map.at(Coordinates(x, y));
}

// TODO
const int weaponSections[3] = { 4, 5, 7 };
const int weaponReach[][14] = {
	{0, 1, 0, 2, 0, 3, 0, 4},
	{0, 1, 0, 2, 1, 0, 1, 1, 2, 0},
	{-1, -1, -1, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1}
};

// TODO
void BattleField::occupy
(SamuraiState& samurai, int direction, Section& pos) {
	const Coordinates& p = pos.coords;
	int weapon = samurai.weapon;
	for (int r = 0; r != weaponSections[weapon]; r++) {
		Coordinates c(weaponReach[weapon][2*r], weaponReach[weapon][2*r+1]);
		Section* section = map.locate(p+c.rotate(direction));
		if (section != 0 && section->homeOf < 0) {
			section->occupy(3*samurai.side+samurai.weapon);
			if (section->population != 0) {
				int a = samurai.side;
				list <int> injured;
				for (int w = 0; w != 3; w++) {
					SamuraiState& ss = gameState.samuraiStates[1-a][w];
					if (ss.position == section) {
						ss.injure(*this);
						injured.push_back(ss.weapon);
					}
				}
			}
		}
	}
}
