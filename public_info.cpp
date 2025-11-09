#include "public_info.h"

#include <iostream>

namespace Utils {


    int levelID = 0;
    std::string WINDOW_TITLE = "chinko finder and his life";

    sf::Vector2i windowPos = sf::Vector2i(0.f, 0.f);

    sf::Time dt{};
    float dtSeconds{};

    std::string talk_text{};

    sf::Vector2<int> get_padding(std::vector<std::vector<int>> map) {

        int padding_top{}, padding_left{};

        int map_heigh = map.size()    * TILE_SIZE * MAP_SCALE;
        int map_width = map[0].size() * TILE_SIZE * MAP_SCALE;

        if (map_heigh < WINDOW_HEIGH) padding_top  = (WINDOW_HEIGH - map_heigh) / 2;
        if (map_width < WINDOW_WIDTH) padding_left = (WINDOW_WIDTH - map_width) / 2;

        return sf::Vector2<int>(padding_top, padding_left);
    }

    std::vector<std::vector<int>> get_map(int id) {

        // 0 - 空气， 1~4 上下左右墙壁，5~8 左上、右上、左下、右下 墙角
        std::vector<std::vector<std::vector<int>>> map_list = {

            {
                {5,1,1,1,1,1,1,6},
                {3,0,0,0,0,0,0,4},
                {3,0,0,0,0,0,0,4},
                {3,0,0,0,0,0,0,4},
                {3,0,0,0,0,0,0,4},
                {3,0,0,0,0,0,0,4},
                {7,2,2,2,2,2,2,8}
            }
        };

        if (id < map_list.size()) return map_list[id];
    }

    std::vector<std::vector<int>> get_item(int id) {

        // 0 - 空气， 1~4 上下左右墙壁，5~8 左上、右上、左下、右下 墙角
        std::vector<std::vector<std::vector<int>>> item_list = {

            {
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,-1,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0}
            }
        };

        if (id < item_list.size()) return item_list[id];
    }

    void print_log(std::string x) { std::cout << x << std::endl; };
}