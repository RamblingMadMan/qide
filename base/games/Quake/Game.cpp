#include "qide/Game.hpp"

class QuakeGame: public qide::Game{
	public:
		std::string_view name() const noexcept override{ return "Quake"; }
		std::string_view author() const noexcept override{ return "Id Software"; }
		qide::Version version() const noexcept override{ return { 0, 0, 0 }; }

		void init() override{}
		void finish() override{}

		void update(float dt) override{}
};

extern "C"
qide::Game *qideGame(){
	static QuakeGame game;
	return &game;
}
