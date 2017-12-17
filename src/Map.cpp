#include "Map.hpp"
#include "Tile.hpp"

#include <fstream>
#include <iostream>

Map::~Map()
{
    //dtor
}

/*
 * Open Binary std::ifstream for the file specified.
 * Set width and height and then we loop enough times to read every
 * Tile from the stream, reading the data upon each loop. read requires a char* as
 * the first argument, which can be interpreted as a pointer to the start of an
 * array of individual bytes (a char is the size of a byte).
 */

void Map::load(const std::string& filename, unsigned int width, unsigned int height, std::map<std::string, Tile>& tileAtlas)
{
    std::ifstream inputFile;
    inputFile.open(filename,std::ios::in | std::ios::binary);

    this->width=width;
    this->height=height;

    for(unsigned int pos=0;pos<this->width * this->height;pos++)
    {
        this->resources.push_back(255);
        this->selected.push_back(0);

        TileType tileType;
        inputFile.read((char*)&tileType,sizeof(int));
        switch(tileType)
        {
            default:
            case TileType::VOID:
            case TileType::GRASS:
                this->tiles.push_back(tileAtlas.at("grass"));
                break;
            case TileType::FOREST:
                this->tiles.push_back(tileAtlas.at("forest"));
                break;
            case TileType::WATER:
                this->tiles.push_back(tileAtlas.at("water"));
                break;
            case TileType::RESIDENTIAL:
                this->tiles.push_back(tileAtlas.at("residential"));
                break;
            case TileType::COMMERCIAL:
                this->tiles.push_back(tileAtlas.at("commercial"));
                break;
            case TileType::INDUSTRIAL:
                this->tiles.push_back(tileAtlas.at("industrial"));
                break;
            case TileType::ROAD:
                this->tiles.push_back(tileAtlas.at("road"));
                break;
            case TileType::SCHOOL:
                this->tiles.push_back(tileAtlas.at("school"));
                break;
            case TileType::HOSPITAL:
                this->tiles.push_back(tileAtlas.at("hospital"));
                break;
        }
        Tile& tile = this->tiles.back();
        inputFile.read((char*)&tile.tileVariant, sizeof(int));
        inputFile.read((char*)&tile.regions, sizeof(int)*1);
        inputFile.read((char*)&tile.population, sizeof(double));
        inputFile.read((char*)&tile.storedGoods, sizeof(float));
    }
    inputFile.close();

    return;
}

void Map::save(const std::string& filename)
{
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out | std::ios::binary);
    for(auto tile : this->tiles)
    {
        outputFile.write((char*)&tile.tileType, sizeof(int));
        outputFile.write((char*)&tile.tileVariant, sizeof(int));
        outputFile.write((char*)&tile.regions, sizeof(int)*3);
        outputFile.write((char*)&tile.population, sizeof(double));
        outputFile.write((char*)&tile.storedGoods, sizeof(float));
    }

    outputFile.close();

    return;
}

/*
 * With the saving and loading functions done, we can write the code to draw the Map.
 * For this we need to know the coordinates of the Tile, and so we use some normal
 * nested for loops (y before x so we iterate horizontally first, and then vertically).
 * We then have some formulae, seemingly conjured out of thin air, that convert the
 * Map coordinates (x,y) to world coordinates that we can use to draw each Tile to
 * the screen.
 */

/* How to ?
 * where the very top point has coordinates (0, 0), the x axis extends along the right
 * edge and the y axis extends along the left edge. If we increase x by 1, then pos.x
 * will increase by tileSize (remember tileSize is half the width, or the height, of
 * each sprite) and pos.y will increase by tileSize * 0.5.
 * http://www.binpress.com/images/uploads/35620/05-01.png
 *
 * pos.x = x * this->tileSize;
 * pos.y = x * this->tileSize * 0.5;
 *
 * If we increase y by 1, then pos.x will decrease by tileSize (follow along the grid lines)
 * and pos.y will increase by tileSize * 0.5.
 *
 * pos.x = x * this->tileSize - y * this->tileSize;
 * pos.y = x * this->tileSize * 0.5 + y * this->tileSize * 0.5;
 *
 * with algebra
 *
 * pos.x = (x - y) * this->tileSize;
 * pos.y = (x + y) * this->tileSize * 0.5;
 *
 */

void Map::draw(sf::RenderWindow& window, float dt,Season season)
{
    for(unsigned int y=0;y<this->height;y++)
    {
        for(unsigned int x=0;x<this->width;x++)
        {
            // Set the Position of the Tile in the 2D World
            sf::Vector2f pos;
            pos.x = (x - y)*this->tileSize + this->width * this->tileSize;
            pos.y = (x + y)*this->tileSize * 0.5;

            this->tiles[y*this->width+x].sprite.setPosition(pos);

            // Change the color if the tile is selected
            if(this->selected[y*this->width+x])
                this->tiles[y*this->width+x].sprite.setColor(sf::Color(0x7d,0x7d,0x7d));
            else if(this->tiles[y*this->width+x].tileType==TileType::FOREST || this->tiles[y*this->width+x].tileType==TileType::GRASS){
                if(season==Season::SPRING) this->tiles[y*this->width+x].sprite.setColor(sf::Color(0xff,0xff,0xff));
                if(season==Season::SUMMER) this->tiles[y*this->width+x].sprite.setColor(sf::Color(0xff,0xd2,0x7f));
                if(season==Season::AUTUMN) this->tiles[y*this->width+x].sprite.setColor(sf::Color(0xff,0xb7,0x32));
                if(season==Season::WINTER) this->tiles[y*this->width+x].sprite.setColor(sf::Color(0xbd,0xbd,0xbd));
            }
            else
                this->tiles[y*this->width+x].sprite.setColor(sf::Color(0xff,0xff,0xff));

            // Draw the tile
            this->tiles[y*this->width+x].draw(window,dt);
        }
    }
    return;
}

/*
 * we clear each Tile's region to 0, and then we iterate over every Tile.
 * Once again we check to see if the tileType is in the whitelist, and if it is and
 * the Tile has not yet been assigned a region we call depthfirstsearch on that tile.
 * Since depthfirstsearch only continues through whitelisted Tiles, every call of
 * depthfirstsearch will be for a new region! Therefore we just increment the
 * regions variable after every call, and each isolated block of tiles will be
 * assigned a different region.
 */
void Map::findConnectedRegions(std::vector<TileType>whitelist, int type=0)
{
    int regions = 1;
    for(auto& tile : this->tiles) tile.regions[type] = 0;
    for(unsigned int y = 0; y < this->height; ++y)
    {
        for(unsigned int x = 0; x < this->width; ++x)
        {
            bool found = false;
            for(auto type : whitelist)
            {
                if(type == this->tiles[y*this->width+x].tileType)
                {
                    found = true;
                    break;
                }
            }
            if(this->tiles[y*this->width+x].regions[type] == 0 && found)
            {
               DepthFirstSearch(whitelist, sf::Vector2i(x, y), regions++, type);
            }
        }
    }
    this->numRegions[type] = regions;
}

/*
 * updateDirection iterates over every Tile. It then builds an array of all the
 * adjacent tiles, setting each element to true if the Tile is of the same type as
 * the centre Tile, and false otherwise. Finally the adjacentTiles array is checked
 * to see the configuration of its true/false values, and the tileVariant of the
 * Tile is set accordingly. The order of the checks here is important, as we are
 * only checking for true elements and not false; some combinations exist that
 * would override others.
 *
 * http://www.binpress.com/images/uploads/35620/06-01.png
 */
void Map::updateDirection(TileType tileType)
{
    for(unsigned int y=0;y<this->height;y++)
    {
        for(unsigned int x=0;x<this->width;x++)
        {
            int pos = y*this->width+x;

            if(this->tiles[pos].tileType!=tileType) continue;
            bool adjacentTiles[3][3] = {{0,0,0},{0,0,0},{0,0,0}};

            /* Check for adjacent tiles of the same type */
            if(x > 0 && y > 0)              adjacentTiles[0][0] = (this->tiles[(y-1)*this->width+(x-1)].tileType == tileType);
            if(y > 0)                       adjacentTiles[0][1] = (this->tiles[(y-1)*this->width+(x  )].tileType == tileType);
            if(x < this->width-1 && y > 0)  adjacentTiles[0][2] = (this->tiles[(y-1)*this->width+(x+1)].tileType == tileType);
            if(x > 0)                       adjacentTiles[1][0] = (this->tiles[(y  )*this->width+(x-1)].tileType == tileType);
            if(x < width-1)                 adjacentTiles[1][2] = (this->tiles[(y  )*this->width+(x+1)].tileType == tileType);
            if(x > 0 && y < this->height-1) adjacentTiles[2][0] = (this->tiles[(y+1)*this->width+(x-1)].tileType == tileType);
            if(y < this->height-1)          adjacentTiles[2][1] = (this->tiles[(y+1)*this->width+(x  )].tileType == tileType);
            if(x < this->width-1 && y < this->height-1) adjacentTiles[2][2] = (this->tiles[(y+1)*this->width+(x+1)].tileType == tileType);

            /* Change the tile variant depending on the tile position */
            if(adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[0][1] && adjacentTiles[2][1])
                this->tiles[pos].tileVariant = 2;
            else if(adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[0][1])
                this->tiles[pos].tileVariant = 7;
            else if(adjacentTiles[1][0] && adjacentTiles[1][2] && adjacentTiles[2][1])
                this->tiles[pos].tileVariant = 8;
            else if(adjacentTiles[0][1] && adjacentTiles[2][1] && adjacentTiles[1][0])
                this->tiles[pos].tileVariant = 9;
            else if(adjacentTiles[0][1] && adjacentTiles[2][1] && adjacentTiles[1][2])
                this->tiles[pos].tileVariant = 10;
            else if(adjacentTiles[1][0] && adjacentTiles[1][2])
                this->tiles[pos].tileVariant = 0;
            else if(adjacentTiles[0][1] && adjacentTiles[2][1])
                this->tiles[pos].tileVariant = 1;
            else if(adjacentTiles[2][1] && adjacentTiles[1][0])
                this->tiles[pos].tileVariant = 3;
            else if(adjacentTiles[0][1] && adjacentTiles[1][2])
                this->tiles[pos].tileVariant = 4;
            else if(adjacentTiles[1][0] && adjacentTiles[0][1])
                this->tiles[pos].tileVariant = 5;
            else if(adjacentTiles[2][1] && adjacentTiles[1][2])
                this->tiles[pos].tileVariant = 6;
            else if(adjacentTiles[1][0])
                this->tiles[pos].tileVariant = 0;
            else if(adjacentTiles[1][2])
                this->tiles[pos].tileVariant = 0;
            else if(adjacentTiles[0][1])
                this->tiles[pos].tileVariant = 1;
            else if(adjacentTiles[2][1])
                this->tiles[pos].tileVariant = 1;
        }
    }

    return;
}

/*
 * We want our industrial zones to dig material from the ground and then ship it to
 * commercial zones where it can be sold. They can't just send the material to any
 * zone though, they would need to be connected using roads (or another zone). To do
 * this we need to check if there is a path that only goes through adjacent zones or
 * roads and that takes us from the industrial zone at the start to some other commercial zone.
 *
 * A depth-first search can do this, which starts at a Tile and checks if we can go
 * through it or not. If we can, it branches off to every adjacent Tile, checking
 * again, before branching off, then checking again... it then stops when we find
 * the Tile we can stop at. This is fine, but it isn't very efficient; we will have
 * to check if every industrial zone is connected to every commercial zone! That's
 * going to be extremely slow, and what's more we have to do it every new game day.
 * One way of improving this would be to simply use a more efficient pathfinding
 * algorithm, such as A*. That doesn't fix our problem though, we've still got far
 * too many pairs of zones to check, especially in a large city.
 *
 * Instead what we do is split the Map into regions (using the regions array from
 * before). Each Tile will be labelled depending on what region it is in, where two
 * Tiles are in the same region if there is a path (through zones or roads) between
 * them. If we find all of those paths, we can just check which region each Tile is
 * in instead of trying to find a path between them each time. (We don't care what
 * the path is after all, only that one exists!)
 */

void Map::DepthFirstSearch(std::vector<TileType>& whiteList, sf::Vector2i pos, int label, int type)
{
    if(pos.x < 0 || pos.x >= this->width) return;
    if(pos.y < 0 || pos.y >= this->height) return;
    if(this->tiles[pos.y*this->width+pos.x].regions[type] != 0) return;
    bool found = false;
    for(auto type : whiteList)
    {
        if(type == this->tiles[pos.y*this->width+pos.x].tileType)
        {
            found = true;
            break;
        }
    }
    if(!found) return;

    this->tiles[pos.y*this->width+pos.x].regions[type] = label;

    DepthFirstSearch(whiteList, pos + sf::Vector2i(-1,  0), label, type);
    DepthFirstSearch(whiteList, pos + sf::Vector2i(0 ,  1), label, type);
    DepthFirstSearch(whiteList, pos + sf::Vector2i(1 ,  0), label, type);
    DepthFirstSearch(whiteList, pos + sf::Vector2i(0 , -1), label, type);

    return;
}

/* Algorithm
 * First we check to see if the supplied position is out of bounds of the Map.
 * If it is, we return. We then check to see if the Tile has already received a
 * region and hence has already been visited by the function. If it has, we return,
 * as we don't want to go over the same Tile twice. If we did the function would
 * never finish! We then check to see if the Tile's tileType is present in whitelist.
 * If it isn't, once again we return, otherwise we assign the Tile a region and call
 * depthfirstsearch again 4 times, once for each adjacent tile.
 */


/*
 * First make sure that the bounding rectangle is extending down and to the right
 * (increasing in both axes) by using the std::swap function (found in the
 * <algorithm> header) to make sure that the start coordinates are smaller than
 * the end ones. We then ensure that the bounding rectangle does not extend off
 * the edges of the map (this would cause a buffer overflow, which we certainly do
 * not want!), before starting at the top left of the rectangle and iterating over
 * every tile within it.
 */


void Map::select(sf::Vector2i start, sf::Vector2i end, std::vector<TileType>blackList)
{
    if(end.y < start.y) std::swap(start.y, end.y);
    if(end.x < start.x) std::swap(start.x, end.x);

    /* Clamp in range */
    if(end.x >= this->width)        end.x = this->width - 1;
    else if(end.x < 0)              end.x = 0;
    if(end.y >= this->height)       end.y = this->height - 1;
    else if(end.y < 0)              end.y = 0;
    if(start.x >= this->width)      start.x = this->width - 1;
    else if(start.x < 0)            start.x = 0;
    if (start.y >= this->height)    start.y = this->height - 1;
    else if(start.y < 0)            start.y = 0;

    for(int y=start.y;y<=end.y;y++)
    {
        for(int x=start.x;x<=end.x;x++)
        {
            // Check if the tile type is in blacklist then invalid else select it
            this->selected[y*this->width+x]=1;
            this->numSelected++;
            for(auto type:blackList)
            {
                if(this->tiles[y*this->width+x].tileType==type)
                {
                    this->selected[y*this->width+x]=2;
                    this->numSelected--;
                    break;
                }
            }
        }
    }

    return;
}

void Map::clearSelected()
{
    for(auto &tile:this->selected)tile=0;
    this->numSelected=0;
}


