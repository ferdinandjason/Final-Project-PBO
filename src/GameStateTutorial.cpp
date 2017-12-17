#include "GameStateTutorial.hpp"

GameStateTutorial::GameStateTutorial(Game *game)
{
    //ctor
    this->game = game;
    sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
    this->view.setSize(pos);
    pos*=0.5f;
    this->view.setCenter(pos);
    this->gameStates=State::TUTOR;

    pos*=2.0f;
    this->guiSystem.emplace("layer",Gui(sf::Vector2f(750,550),0,false,this->game->styleSheets.at("layer"),
        {std::make_pair("","")}));
    pos*=0.5f;
    this->guiSystem.at("layer").setOrigin(375,275);
    this->guiSystem.at("layer").setPosition(pos);
    this->guiSystem.at("layer").show();

    this->guiSystem.emplace("Halaman1",Gui(sf::Vector2f(750,30),10,false,this->game->styleSheets.at("button"),
        {std::make_pair("General Instruction :","0"),
         std::make_pair("1. Right click to show Building option","1"),
         std::make_pair("2. Press and hold left click to select tiles","2"),
         std::make_pair("3. Use mouse wheel to zoom in and out","3"),
         std::make_pair("4. Press mouse wheel to move the camera","4"),
         std::make_pair("5. Move cursor to a building to show building status","5")}));
    this->guiSystem.at("Halaman1").setOrigin(375,15);
    this->guiSystem.at("Halaman1").setPosition(400,50);
    this->guiSystem.at("Halaman1").show();
}

GameStateTutorial::~GameStateTutorial()
{
    //dtor
}

void GameStateTutorial::draw(const float dt)
{
    this->game->window.setView(this->view);

    this->game->window.clear(sf::Color::Black);
    this->game->window.draw(this->game->background);

    for(auto gui:this->guiSystem) this->game->window.draw(gui.second);

    return;
}

void GameStateTutorial::update(const float dt)
{
    sf::Event event;
    sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window),this->view);

    while(this->game->window.pollEvent(event))
    {
        switch(event.type)
        {
            /* Close the window */
            case sf::Event::Closed:
            {
                game->window.close();
                break;
            }
            /* Resize the window */
            case sf::Event::Resized:
            {
                this->view.setSize(event.size.width,event.size.height);
                /* Setiing mouse input for guiStart */
                this->game->background.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0,0)));
                //printf("%d %d\n",this->game->logos.getTexture()->getSize().y,this->game->logos.getTexture()->getSize().x);
                sf::Vector2i koor=sf::Vector2i(event.size.width/2-this->game->logos.getTexture()->getSize().y/2,event.size.height/2-this->game->logos.getTexture()->getSize().x/2);
                koor.y-=90;
                koor.x-=50;
                //printf("%d %d\n",koor.y,koor.x);
                this->game->logos.setPosition(this->game->window.mapPixelToCoords(koor));
                sf::Vector2f pos = sf::Vector2f(event.size.width,event.size.height);
                pos*=0.5f;
                pos=this->game->window.mapPixelToCoords(sf::Vector2i(pos),this->view);
                this->guiSystem.at("menu").setPosition(pos);
                this->game->background.setScale(
                    float(event.size.width)/float(this->game->background.getTexture()->getSize().x),
                    float(event.size.height)/float(this->game->background.getTexture()->getSize().y));
                break;
            }
            /* Highlight menu items */
            case sf::Event::MouseMoved:
            {
                //this->guiSystem.at("menu").highlight(this->guiSystem.at("menu").getEntry(mousePos));
                break;
            }
            /* Click on menu items. */
            case sf::Event::MouseButtonPressed:
            {
                /*if(event.mouseButton.button == sf::Mouse::Left)
                {
                    std::string msg = this->guiSystem.at("menu").activate(mousePos);
                    if(msg == "load_game")
                    {
                        this->loadGame();
                    }
                }*/
                break;
            }
            /* Key Activate */
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape) this->game->window.close();
                else if(event.key.code == sf::Keyboard::H) this->game->popState();
                break;
            }
            default: break;
        }
    }
}

void GameStateTutorial::handleinput()
{

}
