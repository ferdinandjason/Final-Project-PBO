#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>

#include "City.hpp"
#include "Tile.hpp"

City::~City()
{
    //dtor
}

/*
 * In load, we first open an input file stream like we did with Map::load, but this
 * time we don't mark it as a binary file. We then iterate over every line in the file,
 * and create an std::istringstream from the line. This allows us to easily extract
 * data from it. (std::istringstream is like std::ifstream, but for strings and not files.) The file will look like:
 *
 *    width=64
 *    height=64
 *    population=101234
 *
 * We need to split each line up into two parts; one before the '=', and one after.
 * To do this we use the std::getline function again but we pass an extra argument
 * called a delimiter. A delimiter is the character that marks the end of a line,
 * and by default that is just the newline character '\n'. If we set it to '=' however
 * then std::getline will put the first section into the key variable. By calling
 * std::getline once more (with the default delimiter again) we store the second section in value.
 */

void City::load(std::string cityName, std::map<std::string, Tile>& tileAtlas)
{
    int width = 0;
    int height = 0;
    std::ifstream inputFile(cityName + "_cfg.dat", std::ios::in);
    std::string line;
    while(std::getline(inputFile, line))
    {
        std::istringstream lineStream(line);
        std::string key;
        if(std::getline(lineStream, key, '='))
        {
            std::string value;
            if(std::getline(lineStream, value))
            {
                if(key == "width")                  width                   = std::stoi(value);
                else if(key == "height")            height                  = std::stoi(value);
                else if(key == "day")               this->day               = std::stoi(value);
                else if(key == "populationPool")    this->populationPool    = std::stod(value);
                else if(key == "employmentPool")    this->employmentPool    = std::stod(value);
                else if(key == "population")        this->population        = std::stod(value);
                else if(key == "employable")        this->employable        = std::stod(value);
                else if(key == "birthRate")         this->birthRate         = std::stod(value);
                else if(key == "deathRate")         this->deathRate         = std::stod(value);
                else if(key == "residentialTax")    this->residentialTax    = std::stod(value);
                else if(key == "commercialTax")     this->commercialTax     = std::stod(value);
                else if(key == "industrialTax")     this->industrialTax     = std::stod(value);
                else if(key == "funds")             this->funds             = std::stod(value);
                else if(key == "earnings")          this->earnings          = std::stod(value);
            }
            else
            {
                std::cerr << "Error, no value for key " << key << std::endl;
            }
        }
    }

    inputFile.close();
    this->map.load(cityName + "_map.dat", width, height, tileAtlas);
    tileChanged();

    return;
}

/*
 * All that's left is to check key against the possible values and convert the value
 * (which is currently an std::string) into the correct type using std::stod (string to double)
 * and std::stoi (string to int). Once every line has been read we close the file and then
 * load the map. See the "+_map.dat" and "+_cfg.dat"? load should take the name of the City
 * we want to load, say london, and will load the files london_map.dat and london_cfg.dat.
 * save is far simpler and just outputs the correct key and value before saving the map.
 */

void City::save(std::string cityName)
{
    std::ofstream outputFile(cityName + "_cfg.dat", std::ios::out);
    outputFile << "width="              << this->map.width          << std::endl;
    outputFile << "height="             << this->map.height         << std::endl;
    outputFile << "day="                << this->day                << std::endl;
    outputFile << "populationPool="     << this->populationPool     << std::endl;
    outputFile << "employmentPool="     << this->employmentPool     << std::endl;
    outputFile << "population="         << this->population         << std::endl;
    outputFile << "employable="         << this->employable         << std::endl;
    outputFile << "birthRate="          << this->birthRate          << std::endl;
    outputFile << "deathRate="          << this->deathRate          << std::endl;
    outputFile << "residentialTax="     << this->residentialTax     << std::endl;
    outputFile << "commercialTax="      << this->commercialTax      << std::endl;
    outputFile << "industrialTax="      << this->industrialTax      << std::endl;
    outputFile << "funds="              << this->funds              << std::endl;
    outputFile << "earnings="           << this->earnings           << std::endl;
    outputFile.close();
    this->map.save(cityName + "_map.dat");
    return;
}

/*
 * Initially we move to the next day if enough time has passed, much like how AnimationHandler::update
 * works. If a month has passed, then the earnings are added to funds and the earnings reset to 0.
 * The bulk of the update function is a series of loops that iterate over every tile in the map
 * (using shuffledTiles of course). These loops must be separate as the order in which various things happen is important.
 *
 * First, people attempt to move from the populationPool into the residential zones, and the population
 * of each zone is adjusted according to the net birth rate. The commercial zones then attempt to hire
 * people, where they will hire more people the lower the commercialTax is. Finally the industrial zones
 * will attempt to hire people too, but will also extract resources from the ground if any are left.
 *
 * In the second pass, the industrial zones attempt to take any resources from smaller zones that they are
 * connected to via roads or zones (those in the same region) before turning those resources into storedGoods.
 * Each zone can only receive one resource from every other tile, and can only receive one more than its
 * tileVariantileVariant in total resources. The larger the tileVariant (and so the larger and more advanced the
 * industrial zone) the more goods it can produce per resource and the more resources it can receive.
 *
 * In the third and final pass, the goods produced by the industrial zones are distributed amongst the
 * commercial zones which then sell the goods. First, any goods in connected industrial zones are moved
 * into the commercial zone producing taxable income for the industrial zones. Any connected residential
 * regions increase the maximum number of customers the commercial zone can receive. We then calculate
 * the revenue the commercial zones generate from selling the goods.
 *
 * In the last part of update we adjust the populationPool based on the net birth rate, and we add new
 * people to the employmentPool if the total population is different from the population on the previous
 * day. This simulates citizens entering the job market, although of course if the population decreases
 * then the number of employable people will drop too. Finally, we tax all the income (as well as
 * the residential zones) and increase earnings by the total amount.
 */

void City::update(float dt)
{
    double popTotal = 0;
    double commercialRevenue = 0;
    double industrialRevenue = 0;

    /* Update the game time */
    this->currentTime += dt;
    if(this->currentTime < this->timePerDay) return;
    ++day;
    this->currentTime = 0.0;
    if(day % 30 == 0)
    {
        this->funds += this->earnings;
        this->earnings = 0;
    }
    ///////////////////////////////////////////////////////////////////////
    /* Run first pass of tile updates. Mostly handles pool distribution. */
    for(int i = 0; i < this->map.tiles.size()-1; ++i)
    {
        Tile& tile = this->map.tiles[this->shuffledTiles[i]];
        if(tile.tileType == TileType::RESIDENTIAL)
        {
            /* Redistribute the pool and increase the population total by the tile's population */
            this->distributePool(this->populationPool, tile, this->birthRate - this->deathRate);
            popTotal += tile.population;
        }
        else if(tile.tileType == TileType::COMMERCIAL)
        {
            /* Hire people. */
            if(rand() % 100 < 15 * (1.0-this->commercialTax))
                this->distributePool(this->employmentPool, tile, 0.00);
        }
        else if(tile.tileType == TileType::INDUSTRIAL)
        {
            /* Extract resources from the ground. */
            if(this->map.resources[i] > 0 && rand() % 100 < this->population)
            {
                ++tile.productions;
                --this->map.resources[i];
            }
            /* Hire people. */
            if(rand() % 100 < 15 * (1.0-this->industrialTax))
                this->distributePool(this->employmentPool, tile, 0.0);
        }

        tile.update();
    }
    ///////////////////////////////////////////////////////
    /* Run second pass. Mostly handles goods manufacture */
    for(int i = 0; i < this->map.tiles.size()-1; ++i)
    {
        Tile& tile = this->map.tiles[this->shuffledTiles[i]];
        if(tile.tileType == TileType::INDUSTRIAL)
        {
            int receivedResources = 0;
            /* Receive resources from smaller and connected zones */
            for(auto& tile2 : this->map.tiles)
            {
                if(tile2.regions[0] == tile.regions[0] && tile2.tileType == TileType::INDUSTRIAL)
                {
                    if(tile2.productions > 0)
                    {
                        ++receivedResources;
                        --tile2.productions;
                    }
                    if(receivedResources >= tile.tileVariant+1) break;
                }
            }
            /* Turn resources into goods */
            tile.storedGoods += (receivedResources+tile.productions)*(tile.tileVariant+1);
        }
    }
    ////////////////////////////////////////////////////////
    /* Run third pass. Mostly handles goods distribution. */
    for(int i = 0; i < this->map.tiles.size()-1; ++i)
    {
        Tile& tile = this->map.tiles[this->shuffledTiles[i]];
        if(tile.tileType == TileType::COMMERCIAL)
        {
            int receivedGoods = 0;
            double maxCustomers = 0.0;
            for(auto& tile2 : this->map.tiles)
            {
                if(tile2.regions[0] == tile.regions[0] && tile2.tileType == TileType::INDUSTRIAL && tile2.storedGoods > 0)
                {
                    while(tile2.storedGoods > 0 && receivedGoods != tile.tileVariant+1)
                    {
                        --tile2.storedGoods;
                        ++receivedGoods;
                        industrialRevenue += 100 * (1.0-industrialTax);
                    }
                }
                else if(tile2.regions[0] == tile.regions[0] && tile2.tileType == TileType::RESIDENTIAL)
                {
                    maxCustomers += tile2.population;
                }
                if(receivedGoods == tile.tileVariant+1) break;
            }
            /* Calculate the overall revenue for the tile. */
            tile.productions = (receivedGoods*100.0 + rand() % 20) * (1.0-this->commercialTax);
            double revenue = tile.productions * maxCustomers * tile.population / 100.0;
            commercialRevenue += revenue;
        }
    }
    ///////////////////////////////////////////////////
    /* Adjust population pool for births and deaths. */
    this->populationPool += this->populationPool * (this->birthRate - this->deathRate);
    popTotal += this->populationPool;
    //printf("%!!!%lf\n",populationPool);

    /* Adjust the employment pool for the changing population. */
    float newWorkers = (popTotal - this->population) * this->propCanWork;
    newWorkers *= newWorkers < 0 ? -1 : 1;
    this->employmentPool += newWorkers;
    this->employable += newWorkers;
    if(this->employmentPool < 0) this->employmentPool = 0;
    if(this->employable < 0) this->employable = 0;

    /* Update the city population. */
    this->population = popTotal;

    /* Calculate city income from tax. */
    this->earnings = (this->population - this->populationPool) * 15 * this->residentialTax;
    this->earnings += commercialRevenue * this->commercialTax;
    this->earnings += industrialRevenue * this->industrialTax;

    return;
}

/*
 * In the bulldoze function we iterate over every tile in the map. If the
 * tile is selected then we replace it with the given tile and adjust the
 * populationPool if the tile that was destroyed had a population
 *
 * Replace the selected tiles on the map with the tile and
 * update populations etc accordingly
 */

void City::bulldoze(const Tile& tile)
{
    for(int pos = 0; pos < this->map.width * this->map.height; ++pos)
    {
        if(this->map.selected[pos] == 1)
        {
            if(this->map.tiles[pos].tileType == TileType::RESIDENTIAL)
            {
                this->populationPool += this->map.tiles[pos].population;
            }
            else if(this->map.tiles[pos].tileType == TileType::COMMERCIAL)
            {
                this->employmentPool += this->map.tiles[pos].population;
            }
            else if(this->map.tiles[pos].tileType == TileType::INDUSTRIAL)
            {
                this->employmentPool += this->map.tiles[pos].population;
            }
            this->map.tiles[pos] = tile;
        }
    }

    return;
}

/*
 * shuffleTiles is simple but without the aid of std::iota and std::random_shuffle
 * it would be more complicated; first shuffledTiles is created to have the
 * same number of tiles as the map, then std::iota is used to fill shuffledTiles
 * from start to finish with increasing values (starting at 0) before
 * std::random_shuffle is used to randomly move the values about.
 */

void City::shuffleTiles()
{
    while(this->shuffledTiles.size() < this->map.tiles.size())
    {
        this->shuffledTiles.push_back(0);
    }
    std::iota(shuffledTiles.begin(), shuffledTiles.end(), 1);
    std::random_shuffle(shuffledTiles.begin(), shuffledTiles.end());

    return;
}

/*
 * tileChanged first updates all of the roads to face the correct way, before creating regions where roads and zones are connected.
 */

void City::tileChanged()
{
    this->map.updateDirection(TileType::ROAD);
    this->map.findConnectedRegions(
    {
        TileType::ROAD, TileType::RESIDENTIAL,
        TileType::COMMERCIAL, TileType::INDUSTRIAL
    }, 0);

    return;
}

/*
 * distributePool works by moving up to 4 people from the pool into the tile,
 * and then adjusts the tile.population according to the rate passed as an
 * argument. rate will be a birth rate if it's positive and a death rate if
 * it's negative. Most of the code in this function is just to ensure that
 * the right amount of people move and the overall population remains the same.
 */

double City::distributePool(double& pool, Tile& tile, double rate=0.0)
{
    const static int moveRate = 4;
    unsigned int maxPop = tile.maxPopPerLevel*(tile.tileVariant+1);
    /* If there is room in the zone,move up to 4 people from the pool into the zone */
    if(pool>0)
    {
        int moving = maxPop-tile.population;
        if(moving>moveRate) {moving=moveRate;}
        if(pool - moving<0) {moving=pool;}
        pool-=moving;
        tile.population+=moving;
    }
    if(tile.tileType==TileType::RESIDENTIAL)printf("a %lf\n",tile.population);
    if(tile.tileType==TileType::COMMERCIAL)printf("b %lf\n",tile.population);
    if(tile.tileType==TileType::INDUSTRIAL)printf("c %lf\n",tile.population);
    /* Adjust the tile population for birth and deaths */
    tile.population+=tile.population*rate;

    /* Move population that cannot be sustained by the tile into the pool */
    if(tile.population > maxPop)
    {
        pool+=tile.population - maxPop;
        tile.population=maxPop;
    }

    return tile.population;
}

