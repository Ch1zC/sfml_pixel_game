#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Player_Controller {

public:
	Player_Controller(std::string,sf::Vector2i, std::string,bool);
    const bool isThisPlayer()            { return this->isPlayer; }
    const sf::Sprite& get_sprite()       { return this->playerSprite; }
    const std::string get_texture_path() { return this->player_texture_path; }
    const int get_frameSize_x()          { return this->frameSize.x; }
    const int get_frameSize_y()          { return this->frameSize.y; }
    const int get_currentDirection()     { return this->currentDirection; };
    void set_isMovingF ()                { this->isMoving = false  ; }
    void move(std::vector<std::vector<int>>, std::vector<std::vector<int>>);
    void animationPlayer(float, sf::Sprite&);

    enum textureLine
        {
            up    = 0,
            down  = 1,
            left  = 3,
            right = 2
        };
private:

    bool isPlayer;
    std::string name;
    sf::Texture playerTexture;
    sf::Sprite  playerSprite;

	std::string player_texture_path;

    int player_texture_heigh;
    int player_texture_width;

    float player_move_speed;

    bool isMoving;

    int currentDirection;

    float animationTimer;
    float animationFrameDuration;
    int currentFrame;
    int totalFrames;

    sf::Vector2i frameSize;

    sf::Vector2i defPos;
};