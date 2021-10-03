#ifndef QIDE_GAME_HPP
#define QIDE_GAME_HPP 1

#include <string_view>

#include "types.hpp"

namespace qide{
	class Game{
		public:
			virtual ~Game() = default;

			virtual std::string_view name() const noexcept = 0;
			virtual std::string_view author() const noexcept = 0;
			virtual Version version() const noexcept = 0;

			virtual void init(){}
			virtual void finish(){}

			virtual void update(float dt){}
	};
}

extern "C"
qide::Game *qideGame();

#endif // !QIDE_GAME_HPP
