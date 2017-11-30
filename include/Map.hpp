#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Tile.hpp"

/*
 * Map Class : (Game Engine Class I)
 *
 * unsigned int width,height    Dimensions of the map
 * std::vector<Tile>  tiles     Main Map
 * std::vector<int> resources   Industrial zone tiles in order to produce their goods
 * unsigned int tileSize        tile size
 * unsigned int numSelected     is, unsurprisingly, the number of tiles that are currently selected (and are not invalid).
 * unsigned int numRegions      number of regions
 * findConeectingRegions
 * DepthFirstSearch
 * updateDirection              iterates over each Tile in the Map,changing their tileVariant so as to change their animation and orient
 * select                       function that selects all the tiles within the bounding rectangle of start and end, and sets all the tiles within that rectangle that are in the blacklist to invalid.
 * clearSelected                deselect every tile
 *
 */


class Map
{
    public:
        Map()
        {
            this->numSelected=0;
            this->tileSize=0;
            this->width=0;
            this->height=0;
            this->numRegions[0]=1;
        }
        Map(const std::string& filename,unsigned int width,unsigned int height,std::map<std::string,Tile>& tileAtlas)
        {
            this->numSelected=0;
            this->tileSize=8.0;
            load(filename,width,height,tileAtlas);
        }
        ~Map();

        unsigned int width;
        unsigned int height;

        std::vector<Tile> tiles;

        std::vector<int> resources;
        unsigned int tileSize;

        /* 0=Deselected , 1=Selected , 2=Invalid */
        std::vector<char> selected;
        unsigned int numSelected;
        unsigned int numRegions[1];

        /* Select the tiles within the bounds */
        void select(sf::Vector2i start,sf::Vector2i end,std::vector<TileType> blackList);

        void load(const std::string& filename,unsigned int width,unsigned int height,std::map<std::string,Tile>& tileAtlas);
        void save(const std::string& filename);
        void draw(sf::RenderWindow& window,float dt);
        void findConnectedRegions(std::vector<TileType> whitelist,int type);
        void updateDirection(TileType tileType);
        void clearSelected();


    protected:

    private:
        void DepthFirstSearch(std::vector<TileType>& whiteList,sf::Vector2i pos,int label,int type);

};

#endif // MAP_HPP
