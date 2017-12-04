#include "Tile.hpp"

Tile::~Tile()
{
    //dtor
}

std::string tileTypeToStr(TileType type)
{
    switch(type)
    {
        default:
        case TileType::VOID:            return "Void";
        case TileType::GRASS:           return "Flatten";
        case TileType::FOREST:          return "Forest";
        case TileType::WATER:           return "Water";
        case TileType::RESIDENTIAL:     return "Residential Zone";
        case TileType::COMMERCIAL:      return "Commercial Zone";
        case TileType::INDUSTRIAL:      return "Industrial Zone";
    }
}

/*
 * Change the sprite to reflect the tile variant,update the anmation,update sprite,and draw
 *
 * We change the animation to whatever tileVariant is. This is handy as it
 * means that we can place all of the sprites for each tile in a single file,
 * with the animation frames extending to the right and the tile variants
 * extending downwards as separate animations.
 *
 * rand generates a number between 0 and RAND_MAX, 2147483647.
 * Ideally we'd use rand() < 0.1 / (this->tileVariant+1) where rand
 * generates a number between 0 and 1, and so we to take rand() % 10000
 * to get a number from 0 to 10000 and then multiply the other side by 10000
 * to get an equivalent expression using the actual version of rand.
 * If we make the value we mod by larger than RAND_MAX we run into problems,
 * so 10000 is really the largest we can go safely.
 * Essentially the chance is 10% for tileVariant = 0, 5% for tileVariant = 1,
 *  3.33% for tileVariant = 2, and so on.
 */


void Tile::draw(sf::RenderWindow& window, float dt)
{
    this->animHandler.changeAnimation(this->tileVariant);
    this->animHandler.update(dt);

    this->sprite.setTextureRect(this->animHandler.bounds);
    window.draw(this->sprite);

    return;
}

/*
 * If the populatioins is at the maximum value for the tile,
 * there is a small chance that the tile will increase its building stage
 */

void Tile::update()
{
    if((this->tileType == TileType::RESIDENTIAL ||
        this->tileType == TileType::COMMERCIAL ||
        this->tileType == TileType::INDUSTRIAL) &&
        this->population == this->maxPopPerLevel * (this->tileVariant+1) &&
        this->tileVariant < this->maxLevels)
    {
        //if(rand()%int(1e4) < 1e2/(this->tileVariant+1)) ++this->tileVariant;
        this->tileVariant++;
    }

    return;
}
