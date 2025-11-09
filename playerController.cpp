#include "playerController.h"

#include "public_info.h"

#include <SFML/Graphics.hpp>

Player_Controller::Player_Controller(std::string name, sf::Vector2i pos, std::string texture_path,bool isPlayer = false) :
    name(name),
    isPlayer(isPlayer),
	player_texture_path(texture_path),
	player_texture_width(14), // texture总宽28 一行2帧为14
	player_texture_heigh(54),
	player_move_speed(100.f),
	isMoving(false),
	currentDirection(textureLine::down),
	animationTimer(0.f),
	animationFrameDuration(0.4f),
	currentFrame(0),
	totalFrames(2),
	frameSize(player_texture_width, player_texture_heigh)
{

    if (!this->playerTexture.loadFromFile(player_texture_path))
        std::cout << "cant load texture: " << player_texture_path << std::endl;

    // player sprite
    this->playerSprite.setTexture(this->playerTexture);
    this->playerSprite.scale(2.f, 2.f);

    this->playerSprite.setTextureRect(sf::IntRect(0, get_frameSize_y(),
        get_frameSize_x(),
        get_frameSize_y()));
    
    this->playerSprite.setPosition(pos.x,pos.y);
}

void Player_Controller::move(std::vector<std::vector<int>> map, std::vector<std::vector<int>> item_map) {

    sf::Time dt = Utils::dt;
    float dtSeconds = Utils::dtSeconds;

    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        movement.y -= player_move_speed * dt.asSeconds();
        currentDirection = textureLine::up;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        movement.y += player_move_speed * dt.asSeconds();
        currentDirection = textureLine::down;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        movement.x -= player_move_speed * dt.asSeconds();
        currentDirection = textureLine::left;
        isMoving = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        movement.x += player_move_speed * dt.asSeconds();
        currentDirection = textureLine::right;
        isMoving = true;
    }

    sf::Vector2<int> paddingInfo = Utils::get_padding(map);
    int padding_left = paddingInfo.y;
    int padding_top  = paddingInfo.x;


    const int MAP_HEIGHT = map.size();
    const int MAP_WIDTH  = map[0].size();

    sf::Vector2f playerScale   = this->playerSprite.getScale();    // 得到缩放
    sf::Vector2f playerTopLeft = this->playerSprite.getPosition(); // 得到左上角位置

    sf::FloatRect playerPos(
        playerTopLeft.x,
        playerTopLeft.y,
        frameSize.x * playerScale.x,  // 宽度 14 * 2 = 28
        frameSize.y * playerScale.y   // 高度 54 * 2 = 108
    );
    sf::FloatRect playerPos_afterMove = playerPos;
    playerPos_afterMove.left += movement.x;

    float TS_MS = (Utils::TILE_SIZE * Utils::MAP_SCALE);

    int tileX_left   = (int)(playerPos_afterMove.left - padding_left                            ) / TS_MS;
    int tileX_right  = (int)(playerPos_afterMove.left + playerPos_afterMove.width - padding_left) / TS_MS;
    int tileY_top    = (int)(playerPos_afterMove.top  - padding_top                             ) / TS_MS;
    int tileY_bottom = (int)(playerPos_afterMove.top  + playerPos_afterMove.height - padding_top) / TS_MS;

    for (int y = tileY_top; y <= tileY_bottom; ++y)
    {
        for (int x = tileX_left; x <= tileX_right; ++x)
        {
            if (y < 0 || y >= MAP_HEIGHT || x < 0 || x >= MAP_WIDTH) continue;

            if (map[y][x] > 0)
            {

                float wall_left = padding_left + (x * TS_MS);
                float wall_top  = padding_top  + (y * TS_MS);

                sf::FloatRect wallBounds(
                    wall_left,
                    wall_top,
                    TS_MS,
                    TS_MS
                );

                if (playerPos_afterMove.intersects(wallBounds))
                {
                    if (movement.x > 0.f) // 向右
                    {
                        movement.x = wallBounds.left - (playerPos.left + playerPos.width);
                    }
                    else if (movement.x < 0.f) // 向左
                    {
                        movement.x = (wallBounds.left + wallBounds.width) - playerPos.left;
                    }
                    // 动
                    playerPos_afterMove.left = playerPos.left + movement.x;
                }
            }
        }
    }

    this->playerSprite.move(movement.x, 0.f);

    
    playerScale   = this->playerSprite.getScale();
    playerTopLeft = this->playerSprite.getPosition();

    sf::FloatRect playerPos2(
        playerTopLeft.x,
        playerTopLeft.y,
        frameSize.x * playerScale.x,
        frameSize.y * playerScale.y
    );
    playerPos_afterMove = playerPos2;
    playerPos_afterMove.top += movement.y;

    tileX_left   = (int)(playerPos_afterMove.left -                              padding_left) / TS_MS;
    tileX_right  = (int)(playerPos_afterMove.left + playerPos_afterMove.width  - padding_left) / TS_MS;
    tileY_top    = (int)(playerPos_afterMove.top  -                              padding_top ) / TS_MS;
    tileY_bottom = (int)(playerPos_afterMove.top  + playerPos_afterMove.height - padding_top ) / TS_MS;

    for (int y = tileY_top; y <= tileY_bottom; ++y)
    {
        for (int x = tileX_left; x <= tileX_right; ++x)
        {

            if (y < 0 || y >= MAP_HEIGHT || x < 0 || x >= MAP_WIDTH) continue;

            if (map[y][x] > 0)
            {
                float wall_left = padding_left + (x * TS_MS);
                float wall_top = padding_top + (y * TS_MS);

                sf::FloatRect wallBounds(
                    wall_left,
                    wall_top,
                    TS_MS,
                    TS_MS
                );

                if (playerPos_afterMove.intersects(wallBounds))
                {
                    if (movement.y > 0.f) // 向下
                    {
                        movement.y = wallBounds.top - (playerPos2.top + playerPos2.height);
                    }
                    else if (movement.y < 0.f) // 向上
                    {
                        movement.y = (wallBounds.top + wallBounds.height) - playerPos2.top;
                    }
                    // 动
                    playerPos_afterMove.top = playerPos2.top + movement.y;
                }
            }

            /*if (item_map[y][x] < 0) {
                std::cout << "item is at: " + std::to_string(x) +" , " + std::to_string(y) << std::endl;
            }*/
        }
    }

    this->playerSprite.move(0.f, movement.y);

    animationPlayer(dtSeconds, this->playerSprite);

    set_isMovingF();
}

void Player_Controller::animationPlayer(float dtSeconds, sf::Sprite& playerSprite) {

    if (isMoving)
    {
        animationTimer += dtSeconds;
        if (animationTimer >= animationFrameDuration)
        {
            animationTimer -= animationFrameDuration;
            currentFrame = (currentFrame + 1) % totalFrames;
        }
    }
    else
    {
        /*currentFrame = 0;
        currentDirection = textureLine::down;*/
        animationTimer = 0.f;
    }

    playerSprite.setTextureRect(sf::IntRect(
        currentFrame * frameSize.x,     // ver
        currentDirection * frameSize.y, // hor
        frameSize.x,                    // wid
        frameSize.y                     // hei
    ));
}