#pragma once

#include <queue>
#include <SFML/Graphics.hpp>

enum class CommandType {
    TALK,                  // 对话
    SET_GAME_VIEW_VISIBLE, // 游戏世界可见性
    SET_QUEST_STAGE,       // 任务阶段
    GET_QUEST_STAGE
};

struct GameCommand {
    CommandType type;
    std::string string_data_1;
    std::string string_data_2;
    bool bool_data;
    int int_data;

    // 构造函数
    GameCommand(CommandType t) : type(t), bool_data(false), int_data(0) {}
};

namespace Utils {

    constexpr float TILE_SIZE = 54.f;
    constexpr float MAP_SCALE =  2.f;

    constexpr int WINDOW_HEIGH = 830;
    constexpr int WINDOW_WIDTH = 900;
    extern std::string WINDOW_TITLE ;

    extern sf::Time dt;
    extern float dtSeconds;

    extern std::string talk_text;
    extern std::queue<GameCommand> command_queue;

    extern sf::RenderWindow* main_window;

    extern int levelID;

    sf::Vector2<int> get_padding(std::vector<std::vector<int>>);

    std::vector<std::vector<int>> get_map (int id);
    std::vector<std::vector<int>> get_item(int id);

    void print_log(std::string x);
}