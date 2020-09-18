#include "game.h"
#include <memory>

int main()
{
	auto game = std::make_unique<Game>();

	while (!game->quit) {
		// run the game
	}

	return 0;
}
