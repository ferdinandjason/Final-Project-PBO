#ifndef TILE_HPP
#define TILE_HPP

#include <vector>

#include <SFML/Graphics.hpp>

#include "AnimationHandler.hpp"

enum class TileType {VOID,GRASS,FOREST,WATER,RESIDENTIAL,COMMERCIAL,INDUSTRIAL,ROAD};
std::string tileTypeToStr(TileType type);

/*
 * Tile Class : (Game Engine Class II)
 *
 *
 * int titleVariant             allowing for different loking versions of the sam tile
 * unsigned int regions[1]      regions IDs of the tile,tiles in the same region are connected. First is for transport
 * unsigned int cost            placement cost of the tile
 * double populatioin           current residents/employees
 * unsigned int maxPopPerLevel  maximum populations per growth stage / tile variant
 * unsigned maxLevels           maximum number of building levels
 * float production             production output per costumer/worker per day
 * float storedGoods            Goods stored
 */


class Tile
{
    public:
        Tile(){};
        Tile(const unsigned int tileSize,const unsigned int height,sf::Texture& texture,
            const std::vector<Animation>& animations,
            const TileType tileType,const unsigned int cost,const unsigned int maxPopPerLevel,
            const unsigned int maxLevels)
        {
            this->tileType = tileType;
            this->tileVariant = 0;
            this->regions[0] = 0;

            this->cost = cost;
            this->population = 0;
            this->maxPopPerLevel = maxPopPerLevel;
            this->maxLevels = maxLevels;
            this->productions=0;
            this->storedGoods=0;

            this->sprite.setOrigin(sf::Vector2f(0.0f,tileSize*(height-1)));
            this->sprite.setTexture(texture);
            this->animHandler.frameSize=sf::IntRect(0,0,tileSize*2,tileSize*height);
            for(auto animation : animations)
            {
                this->animHandler.addAnimation(animation);
            }
            this->animHandler.update(0.0f);

        }
        ~Tile();

        AnimationHandler animHandler;
        sf::Sprite sprite;

        TileType tileType;

        int tileVariant;
        unsigned int regions[1];
        unsigned int cost;
        double population;
        unsigned int maxPopPerLevel;
        unsigned int maxLevels;
        float productions;
        float storedGoods;

        void draw(sf::RenderWindow& window,float dt);
        void update();

        std::string getCost()
        {
            return std::to_string(this->cost);
        }


    protected:

    private:
};

#endif // TILE_HPP
