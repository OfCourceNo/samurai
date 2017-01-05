#include "players.hpp"

struct SelfPlayer: Player {
  void play(GameInfo& info) {
  }
};

Player* player = new SelfPlayer();
