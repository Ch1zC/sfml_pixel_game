#include "sol/sol.hpp"
#include "public_info.h"
#include "QuestManager.h"
#include "levelsManager.h"
#include "playerController.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <queue>
#include <memory>
#include <vector>
#include <iostream>
#include "Windows.h"

// public_info.h
std::queue<GameCommand> Utils::command_queue;
sf::RenderWindow* Utils::main_window = nullptr;

// printer
float typewriter_timer = 0.f    ;
int   typewriter_index = 0      ;
std::string displayed_text = "" ;
bool  finished_typing  = false  ;

enum class GameState {
    MainMenu,    // 主菜单
    Loading,     // 载入关卡
    cmdToDo,     // 处理命令
    Playing,     // 游戏中
    ReadingLetter, //读信or大段文字什么的
    dialog,      // talk
    dialogDelay, // 说话前延迟状态
    Paused,      // 暂停
    GameOver     // 游戏结束
};

// 说话前留白时间
float dialog_delay_timer = 1.f;

// 说话
void lua_queue_talk(std::string name, std::string text) {
    GameCommand cmd(CommandType::TALK);
    cmd.string_data_1 = name;
    cmd.string_data_2 = text;
    Utils::command_queue.push(cmd);
}

// 场景可见性
void lua_queue_set_view(bool is_visible) {
    GameCommand cmd(CommandType::SET_GAME_VIEW_VISIBLE);
    cmd.bool_data = is_visible;
    Utils::command_queue.push(cmd);
}

// 设置进度
void lua_queue_set_stage(std::string quest_name, int stage) {
    GameCommand cmd(CommandType::SET_QUEST_STAGE);
    cmd.string_data_1 = quest_name;
    cmd.int_data = stage;
    Utils::command_queue.push(cmd);
}

bool display_gameView = true;

int main()
{

    sf::RenderWindow window(sf::VideoMode(Utils::WINDOW_WIDTH , 
                                          Utils::WINDOW_HEIGH),
                                          Utils::WINDOW_TITLE ,
                                          sf::Style::Titlebar | sf::Style::Close);

    Utils::main_window = &window;

    sf::View game_view(window.getDefaultView()); // 显示地图和人物场景
    sf::View ui_view  (window.getDefaultView()); // 显示ui

    // 地图管理
    Levels_Manager levelsManager;

    // 进度管理
    Quest_Manager questManager;

    // 状态管理
    GameState currentState = GameState::Loading;

    // 活动物体
    std::vector<std::unique_ptr<Player_Controller>> x;

    // 时间供计时
    sf::Clock clock;

    // lua实例
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string);

    // 说话的字字体
    sf::Font f;
    if (!f.loadFromFile("fonts/def.ttf")) std::cout << "font cant being load" << std::endl;

    sf::Text t;
    t.setFont(f);
    t.setCharacterSize(22);
    t.setFillColor(sf::Color::White);

    // 说话音效
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("audio/talking.ogg"))
    {
        std::cerr << "cant find the audio file at path: audio/talking.ogg" << std::endl;
        return -1;
    }

    sf::Sound playerSound;
    playerSound.setBuffer(buffer);
    playerSound.setLoop(true);
    playerSound.setVolume(50.f);

    // 主循环
    while (window.isOpen())
    {

        Utils::dt = clock.restart();
        Utils::dtSeconds = Utils::dt.asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {

                if (currentState == GameState::dialog && event.key.code == sf::Keyboard::F && finished_typing)
                {
                    Utils::talk_text = "";
                    Utils::command_queue.pop();
                    
                    currentState = GameState::cmdToDo;
                }
            }
        }

        sf::Vector2f default_center  = ui_view.getCenter();
        sf::Vector2f dialogue_center = default_center;

        dialogue_center.y += 70.f; // 有话说时游戏场景向上移动后的y

        switch (currentState) {
        
            case GameState::MainMenu:
            {
                game_view.setCenter(default_center);
                break;
            }

            case GameState::Loading:
            {
                game_view.setCenter(default_center);

                Utils::print_log("--- loading start");

                levelsManager.characters_init();
                x = std::move(levelsManager.get_sprites_needToDraw()); // sf::sprite 和 sf::texture 不可被复制 移交所属权

                lua.set_function("sys_talk"    , &lua_queue_talk     );
                lua.set_function("sys_setView" , &lua_queue_set_view );
                lua.set_function("sys_setStage", &lua_queue_set_stage);
                lua.set_function("sys_getStage", &Quest_Manager::get_stage, &questManager);

                lua.script_file("scripts/l_test.lua");

                sol::function lua_startGame = lua["startGame"];
                if (lua_startGame.valid()) {
                    lua_startGame();
                }
                
                Utils::print_log("--- loading end");

                currentState = GameState::Playing;
                break;
            }

            case GameState::cmdToDo:
            {

                if (Utils::command_queue.empty())
                {
                    currentState = GameState::Playing;
                    break;
                }

                GameCommand& current_command = Utils::command_queue.front();

                switch (current_command.type) {

                    case CommandType::TALK: {
                
                        dialog_delay_timer = 1.f; // 说话前等待时长
                        currentState = GameState::dialogDelay;
                        break;
                    }
                    case CommandType::SET_GAME_VIEW_VISIBLE: {

                        display_gameView = current_command.bool_data;
                        Utils::command_queue.pop();

                        break;
                    }
                    case CommandType::SET_QUEST_STAGE: {

                        questManager.set_stage(current_command.string_data_1, current_command.int_data);
                        Utils::command_queue.pop();

                        break;
                    }

                }


                break;
            }

            case GameState::Playing:
            {
                game_view.setCenter(default_center);

                if (!Utils::command_queue.empty()) {
                    // 有指令，给到处理器执行

                    currentState = GameState::cmdToDo;
                }
                else {
                    for (int i = 0; i <= x.size() - 1;i++) {
                        x[i]->move(Utils::get_map(Utils::levelID),Utils::get_item(Utils::levelID));
                    }
                }


                break;
            }

            case GameState::dialog: {

                game_view.setCenter(dialogue_center);
            
                if (typewriter_index < Utils::talk_text.length()) {
                    finished_typing = false;

                    const float TIME_PER_CHAR = 0.05f; // 8ms

                    typewriter_timer += Utils::dtSeconds;

                    if (typewriter_timer >= TIME_PER_CHAR) {

                        typewriter_timer -= TIME_PER_CHAR;
                        displayed_text += Utils::talk_text[typewriter_index];
                        typewriter_index++;
                    }
                }
                else {
                
                    finished_typing = true;
                    playerSound.stop();
                }

                break;
            }

            case GameState::dialogDelay: 
            {

                game_view.setCenter(dialogue_center);

                dialog_delay_timer -= Utils::dtSeconds;

                if (dialog_delay_timer <= 0.f) {

                    Utils::talk_text = Utils::command_queue.front().string_data_1 + ": " + Utils::command_queue.front().string_data_2;

                    displayed_text   = "" ;
                    typewriter_index = 0  ;
                    typewriter_timer = 0.f;

                    playerSound.play();

                    currentState = GameState::dialog;
                }
                

                break;
            }

            case GameState::Paused:
            {
                break;
            }

            case GameState::GameOver:
            {
                break;
            }
        }

        //渲染

        window.clear();      // 清除上一帧

        window.setView(game_view);
        
        // 场景 --- START ---

        //currentState == GameState::Playing
        if (display_gameView) {

            levelsManager.show_level(window);

            for (int i = 0; i <= x.size() - 1;i++) {
                window.draw(x[i]->get_sprite());
            }
        }
        else if (currentState == GameState::ReadingLetter) {

            // 显示信件
        }

        // 场景 --- END ---

        window.setView(ui_view);

        // UI --- START ---

        if (!Utils::talk_text.empty()) {
        
            t.setString(displayed_text);
            t.setPosition((Utils::WINDOW_WIDTH - t.getGlobalBounds().width) / 2,
                Utils::WINDOW_HEIGH - 100);

            window.draw(t);
        }

        // UI --- END ---

        window.display();    // 正式显示
    }

    return 0;
}

/*

背包效果可以直接一个item一个window，关闭window的话等于丢掉item，用掉item的话自动关闭对应window

*/