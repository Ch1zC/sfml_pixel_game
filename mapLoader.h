#pragma once

#include <vector>
#include <iostream>

#include "public_info.h"

class Map_Loader {

public:
	Map_Loader();
	std::vector<sf::Sprite> getMapSprites_asVector(sf::RenderWindow&, int);

private:
	sf::Texture wall_texture;
	sf::Texture corner_texture;
	sf::Texture letter_texture;
	sf::Sprite tileSprite;
	sf::Texture wall_c_texture;

	std::string wall_texture_path;
	std::string wall_c_texture_path;
	std::string corner_texture_path;
	std::string texture_parent_path;
	std::string letter_texture_path;
	const float TILE_ORIGIN = Utils::TILE_SIZE / 2.f;
};