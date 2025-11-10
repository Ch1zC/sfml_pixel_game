#include "sol/sol.hpp"
#include "public_info.h"
#include "QuestManager.h"
#include "levelsManager.h"
#include "item_database.h"
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

int targetGridX = 0;
int targetGridY = 0;

std::string letter_text = "";

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
void lua_queue_talk(int id, std::string text, sol::optional<float> vol, sol::optional<int> size) {
    GameCommand cmd(CommandType::TALK);
    cmd.int_data_1 = id;
    cmd.int_data_2 = size.value_or(25);
    cmd.float_data_1 = vol.value_or(100.f);
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
    cmd.int_data_1 = stage;
    Utils::command_queue.push(cmd);
}

std::vector<sf::Color> f_color_list = {sf::Color::White, sf::Color::Green};
std::vector<sf::SoundBuffer> s_audio_list;

bool display_gameView = true;
bool aItemNearbyPlayer = false;

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

    // 道具管理
    Item_Database itemDatabase;

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

    sf::Color f_color{};
    sf::Font f;
    if (!f.loadFromFile("fonts/def.ttf")) std::cout << "font cant being load" << std::endl;

    sf::Text t;
    t.setFont(f);
    
    // 信的字体
    sf::Text t_letter;
    t_letter.setFont(f);
    t_letter.setCharacterSize(40);
    t_letter.setFillColor(sf::Color(0x533461FF));

    // 说话音效 
    // - talking1
    sf::SoundBuffer s_audio{};

    sf::SoundBuffer a_talking1;
    if (!a_talking1.loadFromFile("audio/talking.ogg"))
    {
        std::cerr << "cant find the audio file at path: audio/talking.ogg" << std::endl;
        return -1;
    }
    s_audio_list.push_back(a_talking1);

    // - talking2
    sf::SoundBuffer a_talking2;
    if (!a_talking2.loadFromFile("audio/talking2.ogg"))
    {
        std::cerr << "cant find the audio file at path: audio/talking2.ogg" << std::endl;
        return -1;
    }
    s_audio_list.push_back(a_talking2);

    sf::Sound s_talking;
    s_talking.setLoop(true);

    // - pick up letter
    sf::SoundBuffer a_pick_up_letter;
    if (!a_pick_up_letter.loadFromFile("audio/pick_up_letter.wav"))
    {
        std::cerr << "cant find the audio file at path: audio/pick_up_letter.ogg" << std::endl;
        return -1;
    }

    sf::Sound sound_pick_up_letter;
    sound_pick_up_letter.setBuffer(a_pick_up_letter);
    sound_pick_up_letter.setLoop(false);
    sound_pick_up_letter.setVolume(60.f);


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

                    Utils::print_log("++++ contuine talking");
                    
                    currentState = GameState::cmdToDo;
                }

                // 退出读信状态
                else if (currentState == GameState::ReadingLetter && event.key.code == sf::Keyboard::F)
                {
                    letter_text = "";
                    display_gameView = true;

                    std::string itemID = std::to_string(Utils::levelID) + "_" + std::to_string(targetGridX) + "_" + std::to_string(targetGridY);
                    questManager.set_flag("read_" + itemID, true);

                    Utils::print_log("set flag: read_" + itemID + "-> true");

                    Utils::print_log("++++ finished reading");

                    currentState = GameState::Playing;

                    //检查条件
                    sol::function lua_checkQuests = lua["checkQuests"];
                    if (lua_checkQuests.valid()) {
                        lua_checkQuests();
                        Utils::print_log("FFFF lua running checkQuests");
                    }
                }

                // 进入读信
                else if (currentState == GameState::Playing && aItemNearbyPlayer && event.key.code == sf::Keyboard::F) {

                    sound_pick_up_letter.play();

                    Utils::print_log("++++ start reading");

                    std::string itemID = std::to_string(Utils::levelID) + "_" + std::to_string(targetGridX) + "_" + std::to_string(targetGridY);

                    letter_text = itemDatabase.get_itemInfoByID(itemID).content;

                    for (int i = 34; i <= letter_text.length(); i += 34) {

                        std::string x = letter_text[i] == ' ' ? "\n" : "-\n-";
                        letter_text = letter_text.substr(0, i) + x + letter_text.substr(i, letter_text.length() - 1);
                    }

                    display_gameView = false;

                    currentState = GameState::ReadingLetter;

                } // 进入读信 end

            } // 键盘监控 end

        } // 按键事件传递 end

        sf::Vector2f default_center  = ui_view.getCenter();
        sf::Vector2f dialogue_center = default_center;

        dialogue_center.y += 30.f; // 有话说时游戏场景向上移动后的y

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
                lua.set_function("sys_getFlag" , &Quest_Manager::get_flag, &questManager);

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

                        t.setFillColor(f_color_list[current_command.int_data_1]);
                        t.setCharacterSize(current_command.int_data_2);

                        s_talking.setBuffer(s_audio_list[current_command.int_data_1]);
                        s_talking.setVolume(current_command.float_data_1);

                        currentState = GameState::dialogDelay;
                        break;
                    }
                    case CommandType::SET_GAME_VIEW_VISIBLE: {

                        display_gameView = current_command.bool_data;
                        Utils::command_queue.pop();

                        break;
                    }
                    case CommandType::SET_QUEST_STAGE: {

                        questManager.set_stage(current_command.string_data_1, current_command.int_data_1);
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

                    // 检测用户所在位置是否有item可pick up
                    Player_Controller* player = x[0].get();

                    std::vector<std::vector<int>> item_map = Utils::get_item(Utils::levelID);
                    std::vector<std::vector<int>> map      = Utils::get_map (Utils::levelID);

                    sf::Vector2<int> paddingInfo = Utils::get_padding(map);

                    // 计算player中心点所在坐标
                    sf::FloatRect bounds = player->get_sprite().getGlobalBounds();

                    float playerCenterX  = bounds.left + bounds.width  / 2.f;
                    float playerCenterY  = bounds.top  + bounds.height / 2.f;

                    float TS_MS = (Utils::TILE_SIZE * Utils::MAP_SCALE);

                    int playerGridX = (int)(playerCenterX - paddingInfo.y) / TS_MS;
                    int playerGridY = (int)(playerCenterY - paddingInfo.x) / TS_MS;


                    targetGridX = playerGridX;
                    targetGridY = playerGridY;

                    int item_id = item_map[targetGridY][targetGridX];

                    if (item_id < 0) {

                        // there is a item
                        //std::cout << "there is a item that you can take at (" << targetGridX << "," << targetGridY << ")" << std::endl;

                        if (!aItemNearbyPlayer) aItemNearbyPlayer = true;
                    }
                    else {
                        // 等玩家到了没item的地方再设为false
                        targetGridX = 0;
                        targetGridY = 0;
                        if (aItemNearbyPlayer) aItemNearbyPlayer = false;
                    }
                }


                break;
            }

            case GameState::dialog: {

                //game_view.setCenter(dialogue_center);
            
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
                    s_talking.stop();
                }

                break;
            }

            case GameState::dialogDelay: 
            {

                //game_view.setCenter(dialogue_center);

                dialog_delay_timer -= Utils::dtSeconds;

                if (dialog_delay_timer <= 0.f) {

                    // 别人说话换其他说话声音和文字颜色，主角则是白色和def.ogg
                    Utils::talk_text = Utils::command_queue.front().string_data_2;

                    displayed_text   = "" ;
                    typewriter_index = 0  ;
                    typewriter_timer = 0.f;

                    s_talking.play();

                    currentState = GameState::dialog;
                }
                

                break;
            }

            case GameState::ReadingLetter: 
            {

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
            t_letter.setString(letter_text);
            t_letter.setPosition(50.f,50.f);

            window.draw(t_letter);
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

灵感:

1 - 背包效果可以直接一个item一个window，关闭window的话等于丢掉item，用掉item的话自动关闭对应window

2 - 直接画所有地图不过限制game_view可视范围，如果玩家坐标超出了范围得到玩家朝哪个方向走的然后向那个方向移动以一个地图的距离实现跨地图
*/