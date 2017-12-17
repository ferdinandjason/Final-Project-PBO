#include <stack>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Game.hpp"
#include "GameState.hpp"

Game::Game()
{
    //ctor
    this->loadTextures();
    this->loadTiles();
    this->loadFonts();
    this->loadSytleSheets();

    this->window.create(sf::VideoMode(800,600),"City Builder");
    this->window.setFramerateLimit(60);

    this->background.setTexture(this->texmgr.getRef("background"));
    this->backgroundStart.setTexture(this->texmgr.getRef("backgrondStart"));
    this->logos.setTexture(this->texmgr.getRef("logos"));
}

Game::~Game()
{
    //dtor
    while(!this->states.empty()) popState();
}

void Game::pushState(GameState* state)
{
    this->states.push(state);

    return;
}

void Game::popState()
{
    if(this->states.top()->gameStates!=State::PAUSE && this->states.top()->gameStates!=State::TUTOR) delete this->states.top();
    this->states.pop();

    return;
}

void Game::changeState(GameState* state)
{
    if(!this->states.empty()) popState();
    pushState(state);

    return;
}

GameState* Game::peekState()
{
    if(!this->states.empty()) return this->states.top();
    return nullptr;
}

void Game::run()
{
    sf::Clock clock;

    while(this->window.isOpen())
    {
        sf::Time elapsed=clock.restart();
        float dt=elapsed.asSeconds();
        dt*=Accelerator;
        if(peekState()==nullptr) continue;
        peekState()->handleinput();
        peekState()->update(dt);
        this->window.clear(sf::Color::Black);
        peekState()->draw(dt);
        this->window.display();
    }
}


void Game::loadTextures()
{
    texmgr.loadTexture("grass",         "media/grass.png");
    texmgr.loadTexture("forest",        "media/forest.png");
    texmgr.loadTexture("water",         "media/water.png");
    texmgr.loadTexture("residential",   "media/residential.png");
    texmgr.loadTexture("commercial",    "media/commercial.png");
    texmgr.loadTexture("industrial",    "media/industrial.png");
    texmgr.loadTexture("road",          "media/road.png");
    texmgr.loadTexture("school",        "media/school.png");
    texmgr.loadTexture("hospital",      "media/hospital.png");

    texmgr.loadTexture("background",    "media/background.png");
    texmgr.loadTexture("backgrondStart","media/backgroundStart.png");
    texmgr.loadTexture("logos",         "media/logos.png");
}


void Game::loadTiles()
{
    Animation staticAnim(0, 0, 1.0f);
    this->tileAtlas["grass"] =
        Tile(this->tileSize, 1, texmgr.getRef("grass"),
            { staticAnim },
            TileType::GRASS, 50, 0, 1);
    tileAtlas["forest"] =
        Tile(this->tileSize, 1, texmgr.getRef("forest"),
            { staticAnim },
            TileType::FOREST, 100, 0, 1);
    tileAtlas["water"] =
        Tile(this->tileSize, 1, texmgr.getRef("water"),
            { Animation(0, 3, 0.5f),
              Animation(0, 3, 0.5f),
              Animation(0, 3, 0.5f)
            },TileType::WATER, 0, 0, 1);
    tileAtlas["residential"] =
        Tile(this->tileSize, 2, texmgr.getRef("residential"),
            { staticAnim, staticAnim, staticAnim,
            staticAnim, staticAnim, staticAnim },
            TileType::RESIDENTIAL, 300, 50, 6);
    tileAtlas["commercial"] =
        Tile(this->tileSize, 2, texmgr.getRef("commercial"),
            { staticAnim, staticAnim, staticAnim, staticAnim},
            TileType::COMMERCIAL, 300, 50, 4);
    tileAtlas["industrial"] =
        Tile(this->tileSize, 2, texmgr.getRef("industrial"),
            { staticAnim, staticAnim, staticAnim, staticAnim },
            TileType::INDUSTRIAL, 300, 50, 4);
    tileAtlas["road"] =
        Tile(this->tileSize, 1, texmgr.getRef("road"),
            { staticAnim, staticAnim, staticAnim,
            staticAnim, staticAnim, staticAnim,
            staticAnim, staticAnim, staticAnim,
            staticAnim, staticAnim },
            TileType::ROAD, 100, 0, 1);
    tileAtlas["school"] =
        Tile(this->tileSize, 2, texmgr.getRef("school"),
            { staticAnim },
            TileType::SCHOOL, 1000, 0, 0);
    tileAtlas["hospital"] =
        Tile(this->tileSize, 2, texmgr.getRef("hospital"),
            { staticAnim },
            TileType::HOSPITAL, 1000, 0, 0);

    return;
}

void Game::loadSytleSheets()
{
    this->styleSheets["button"] = GuiStyle(&this->fonts.at("main_font"),1,
        sf::Color(0xc6,0xc6,0xc6),sf::Color(0x94,0x94,0x94), sf::Color(0x00,0x00,0x00),
        sf::Color(0x61,0x61,0x61),sf::Color(0x94,0x94,0x94), sf::Color(0x00,0x00,0x00));
    this->styleSheets["button2"] = GuiStyle(&this->fonts.at("pixel_font"),1,
        sf::Color(0xc6,0xc6,0xc6),sf::Color(0x94,0x94,0x94), sf::Color(0x00,0x00,0x00),
        sf::Color(0x61,0x61,0x61),sf::Color(0x94,0x94,0x94), sf::Color(0x00,0x00,0x00));
    this->styleSheets["text"] = GuiStyle(&this->fonts.at("main_font"), 0,
        sf::Color(0x00,0x00,0x00,0x00), sf::Color(0x00,0x00,0x00), sf::Color(0xff,0xff,0xff),
        sf::Color(0x00,0x00,0x00,0x00), sf::Color(0x00,0x00,0x00), sf::Color(0xff,0x00,0x00));
    this->styleSheets["text2"] = GuiStyle(&this->fonts.at("pixel_font"), 0,
        sf::Color(0x00,0x00,0x00,0x00), sf::Color(0x00,0x00,0x00), sf::Color(0xff,0xff,0xff),
        sf::Color(0x00,0x00,0x00,0x00), sf::Color(0x00,0x00,0x00), sf::Color(0xff,0x00,0x00));
    this->styleSheets["layer"] = GuiStyle(&this->fonts.at("pixel_font"),0,
        sf::Color(0x69,0x69,0x69,0x99),sf::Color(0x69,0x69,0x69,0x99),sf::Color(0x69,0x69,0x69,0x99),
        sf::Color(0x69,0x69,0x69,0x99),sf::Color(0x69,0x69,0x69,0x99),sf::Color(0x69,0x69,0x69,0x99));
    this->styleSheets["status"] = GuiStyle(&this->fonts.at("main_font"),0,
        sf::Color(0x00,0x00,0x00,0x99),sf::Color(0x00,0x00,0x00,0x99),sf::Color(0xff,0xff,0xff,0x99),
        sf::Color(0x00,0x00,0x00,0x99),sf::Color(0x00,0x00,0x00,0x99),sf::Color(0x00,0x00,0x00,0x99));

        return;
}

void Game::loadFonts()
{
    sf::Font font;
    font.loadFromFile("media/font.ttf");
    this->fonts["main_font"] = font;
    sf::Font font2;
    font2.loadFromFile("media/pixel.ttf");
    this->fonts["pixel_font"] = font2;
    return;
}
