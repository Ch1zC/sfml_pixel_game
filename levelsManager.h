#pragma once

// #include <vector>
#include <SFML/Graphics.hpp>
#include "playerController.h"


class Levels_Manager {
public:
	Levels_Manager();
	void characters_init();
	void show_level(sf::RenderWindow&);
	std::vector<std::unique_ptr<Player_Controller>> get_sprites_needToDraw() { return std::move(this->characters_needToDraw);  }
	//void to_nextLevel() { map_need_to_show_id++; show_level(map_need_to_show_id); };

private:

	std::vector<std::unique_ptr<Player_Controller>> characters_needToDraw;
};