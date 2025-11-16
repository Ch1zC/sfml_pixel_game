#include "public_info.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Utils {

    std::vector<std::vector<int>> map_showing{};
    std::vector<std::vector<int>> item_map_showing{};

    std::string map_path_showing{};
    std::string item_map_path_showing{};

    int levelID = 0;
    std::string WINDOW_TITLE = "a short horror game";

    sf::Vector2i windowPos = sf::Vector2i(0.f, 0.f);

    sf::Time dt{};
    float dtSeconds{};

    std::string talk_text{};

    sf::Vector2<int> item_nearby(0,0);

    std::string map_folder_path = "map/";
    std::string item_map_folder_path = "map/item_map/";

    sf::Vector2<int> get_padding(std::vector<std::vector<int>> map) {

        int padding_top{}, padding_left{};

        int map_heigh = map.size()    * TILE_SIZE * MAP_SCALE;
        int map_width = map[0].size() * TILE_SIZE * MAP_SCALE;

        if (map_heigh < WINDOW_HEIGH) padding_top  = (WINDOW_HEIGH - map_heigh) / 2 - 30;
        if (map_width < WINDOW_WIDTH) padding_left = (WINDOW_WIDTH - map_width) / 2;

        return sf::Vector2<int>(padding_top, padding_left);
    }

    void get_map() {

        std::string p = map_folder_path + map_path_showing;

        std::ifstream inputFile(p); // 打开文件
        std::string line;

        std::vector<std::vector<int>> result_l;

        if (inputFile.is_open()) {
            while (std::getline(inputFile, line)) {

                std::vector<int> l;
                std::stringstream ss(line);
                std::string segment;

                while (std::getline(ss, segment, ',')) // 按 ',' 分割
                {
                    int v = 0;
                    try {
                        v = std::stoi(segment); // std::stoi 会自动处理 " 0"
                    }
                    catch (...) {
                        v = 0;
                    }
                    l.push_back(v);
                }
                
                result_l.push_back(std::move(l));
            }
            inputFile.close();
            print_log("loaded level file: " + p);

            map_showing = result_l;
        }
        else {
            print_log("cant load level file: " + p);
        }
    }

    void get_item() {

        std::string p = item_map_folder_path + item_map_path_showing;

        std::ifstream inputFile(p); // 打开文件
        std::string line;

        std::vector<std::vector<int>> result_l;

        if (inputFile.is_open()) {
            while (std::getline(inputFile, line)) {

                std::vector<int> l;
                std::stringstream ss(line);
                std::string segment;

                while (std::getline(ss, segment, ',')) // 按 ',' 分割
                {
                    int v = 0;
                    try {
                        v = std::stoi(segment); // std::stoi 会自动处理 " 0"
                    }
                    catch (...) {
                        v = 0;
                    }
                    l.push_back(v);
                }

                result_l.push_back(std::move(l));
            }
            inputFile.close();
            print_log("loaded item level file: " + p);

            item_map_showing = result_l;
        }
        else {
            print_log("cant load item level file: " + p);
        }
    }

    void print_log(std::string x) { std::cout << x << std::endl; };
}