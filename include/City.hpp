#ifndef CITY_HPP
#define CITY_HPP

#include "Map.hpp"

/*
 * City Class : (Actual Main Game Engine)
 *
 * City()                           Constructor
 * void load(),save()               will load and save the City from files respectively (loading and saving the map too),
 * void update                      update will move people around, calculate income, move goods around and so on
 * void bulldoze                    will replace the selected (and valid) area of map with tile,
 * void shuffleTile                 will generate the shuffledTiles std::vector
 * void tileChanged                 will update the regions and directions of tiles and should of course be called whenever a Tile is changed,
 *
 * void distributePool              will be used in update to move citizens around
 *
 * float currentTime                currentTime is the real world time (in seconds) since the day updated,
 * float timePerDay                 timePerDay is the amount of real world time each day should last. We've set this to 1.0 in the constructor to get a 1:1 correspondence of seconds to days.
 *                                  The game world will update at the end of each day, so the lower this value the faster the game will go
 * std::vector<int> shuffledTiles   which has an interesting use; if we were to update the tiles by iterating over them they would update from left to right and top to bottom on the map. with update function
 * double populationPool            Number of residents who are not in a residential zone.
 * double employmenyPool            Number of residents who are not currently employed but can work
 * float propCanWork                Proportion of resident who can work
 * double birthRate,deathRate       Proportion of residents who die/give birth each day, Estimate for death rate = 1/(life expectancy*360), current world values are 0.000055 and 0.000023, respectively
 *
 * Map map                          Store map of the Game
 * double population                Number of population
 * double employable                Number og employable population
 * double residentialTax            store the proportion of income from each zone that is taxed by the City
 * double commercialTax             store the proportion of income from each zone that is taxed by the City
 * double industrialTax             store the proportion of income from each zone that is taxed by the City
 * double earnings,funds            Running total of city earnings this month
 *
 * Now for a brief explanation on how population will work. The City has a populationPool, which stores the number of citizens who do not have a home.
 * Each Tile has a population value (as we've seen) that stores the number of citizens living within. So to move people into houses we decrease populationPool
 * and increase population. The total population of the City is calculated as the sum of all the Tiles' populations and the populationPool. The same applies for
 * employable and employmentPool, but those are for commercial and industrial zones and not residential ones.
 *
 * We then have propCanWork, which is the proportion of the population that can work and thus can be employed. Thus employable is approximately equal to
 * propCanWork * population. Next we have birthRate and deathRate, which are set to be 100 times the real world value in order to speed up gameplay.
 * Or you could just make the days run faster and keep them the same, of course.
 */

class City
{
    public:
        City()
        {
            this->birthRate = 0.00055;
            this->deathRate = 0.00023;
            this->propCanWork = 0.50;
            this->populationPool = 0;
            this->population = populationPool;
            this->employmentPool = 0;
            this->employable = employmentPool;
            this->residentialTax = 0.05;
            this->commercialTax = 0.05;
            this->industrialTax = 0.05;
            this->earnings = 0;
            this->funds = 0;
            this->currentTime = 0.0;
            this->timePerDay = 1.0;
            this->day = 0;
        }
        City(std::string cityName,int tileSize,std::map<std::string,Tile>& tileAtlas):City()
        {
            this->map.tileSize=tileSize;
            load(cityName,tileAtlas);
        }
        ~City();

        void load(std::string cityName,std::map<std::string,Tile>& tileAtlas);
        void save(std::string cityName);

        void update(float dt);
        void bulldoze(const Tile& tile);
        void shuffleTiles();
        void tileChanged();

        double getHomeless() {return this->populationPool;}
        double getUnemployed() {return this->employmentPool;}

        Map map;
        double population;
        double employable;

        double residentialTax;
        double commercialTax;
        double industrialTax;

        double earnings;
        double funds;

        int day;
        int hospitalCounter=0;
        int schoolCounter=0;

        float propCanWork;
        double birthRate;
        double deathRate;

    protected:

    private:
        float currentTime;
        float timePerDay;

        std::vector<int> shuffledTiles;

        double populationPool;
        double employmentPool;


        double distributePool(double& pool,Tile& tile,double rate);
};

#endif // CITY_HPP
