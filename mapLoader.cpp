#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "mapLoader.h"
#include "public_info.h"

Map_Loader::Map_Loader() :
	wall_texture_path("wall_hor.png"),
	corner_texture_path("wall_unhor.png"),
    letter_texture_path("letter.png"),
    texture_parent_path("textures/")
{

    if (!wall_texture.loadFromFile(texture_parent_path + wall_texture_path))
        std::cout << "cant load texture: " << texture_parent_path + wall_texture_path << std::endl;

    if (!corner_texture.loadFromFile(texture_parent_path + corner_texture_path))
        std::cout << "cant load texture: " << texture_parent_path + corner_texture_path << std::endl;

    if (!letter_texture.loadFromFile(texture_parent_path + letter_texture_path))
        std::cout << "cant load texture: " << texture_parent_path + letter_texture_path << std::endl;

    tileSprite.scale(Utils::MAP_SCALE, Utils::MAP_SCALE);
}

std::vector<sf::Sprite> Map_Loader::getMapSprites_asVector(sf::RenderWindow& window, int mapID) {

    std::vector<sf::Sprite> sprites_list;

    std::vector<std::vector<int>> map = Utils::get_map(mapID);
    std::vector<std::vector<int>> item_map = Utils::get_item(mapID);

    sf::Vector2<int> paddingInfo = Utils::get_padding(map);

    sf::Vector2f defScale = tileSprite.getScale();

	for (int y = 0; y < map.size(); y++) {
		
		for (int x = 0; x < map[y].size();x++) {

            tileSprite.setScale(defScale.x, defScale.y);
			
			int blockID = map[y][x];

            if (blockID != 0) {

                // 计算block圆心在世界中的坐标
                float posX = paddingInfo.y + ((x * Utils::TILE_SIZE) + TILE_ORIGIN) * Utils::MAP_SCALE;
                float posY = paddingInfo.x + ((y * Utils::TILE_SIZE) + TILE_ORIGIN) * Utils::MAP_SCALE;

                tileSprite.setPosition(posX, posY);
                tileSprite.setOrigin(TILE_ORIGIN, TILE_ORIGIN);

                switch (blockID)
                {
                case 1: // 上
                    tileSprite.setTexture(wall_texture);
                    tileSprite.setRotation(0);
                    break;

                case 2: // 下
                    tileSprite.setTexture(wall_texture);
                    tileSprite.setRotation(180); // 转180
                    break;

                case 3: // 左
                    tileSprite.setTexture(wall_texture);
                    tileSprite.setRotation(270); // 转270
                    break;

                case 4: // 右
                    tileSprite.setTexture(wall_texture);
                    tileSprite.setRotation(90); // 转90
                    break;

                    //拐角

                case 5: // 左上
                    tileSprite.setTexture(corner_texture);
                    tileSprite.setRotation(0);
                    break;

                case 6: // 右上
                    tileSprite.setTexture(corner_texture);
                    tileSprite.setRotation(90);
                    break;

                case 7: // 左下
                    tileSprite.setTexture(corner_texture);
                    tileSprite.setRotation(270);
                    break;

                case 8: // 右下
                    tileSprite.setTexture(corner_texture);
                    tileSprite.setRotation(180);
                    break;
                }

                sprites_list.push_back(tileSprite);
            }

            int itemID = item_map[y][x];

            if (itemID != 0) {

                float posX = paddingInfo.y + ((x * Utils::TILE_SIZE) + TILE_ORIGIN) * Utils::MAP_SCALE;
                float posY = paddingInfo.x + ((y * Utils::TILE_SIZE) + TILE_ORIGIN) * Utils::MAP_SCALE;

                tileSprite.setPosition(posX, posY);
                tileSprite.setOrigin(TILE_ORIGIN, TILE_ORIGIN);
                
                switch (itemID) {
                    case -1: {
                        tileSprite.setScale(0.9f, 0.9f);
                        tileSprite.setTexture(letter_texture);
                        tileSprite.setRotation(0);
                        break;
                    }
                }

                sprites_list.push_back(tileSprite);

            }
            
		}
	}
    return sprites_list;
}