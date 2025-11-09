#include "levelsManager.h"

#include "mapLoader.h"
#include "playerController.h"

Levels_Manager::Levels_Manager()
{}

void Levels_Manager::characters_init() {
	// 读取关卡文件并写入关卡信息

	characters_needToDraw.push_back(
		std::make_unique<Player_Controller>("You" ,sf::Vector2i(340.f, 500.f), "textures/player_move.png", true)
	);
}

void Levels_Manager::show_level(sf::RenderWindow& window) {

	Map_Loader mapLoader;

	//背景
	std::vector<sf::Sprite> MapSprites = mapLoader.getMapSprites_asVector(window, Utils::levelID);

	for (int i = 0;i < MapSprites.size();i++) {
		window.draw(MapSprites[i]);
	}
}