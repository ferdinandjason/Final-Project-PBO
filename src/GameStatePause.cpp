#include "GameStatePause.hpp"

GameStatePause::GameStatePause(Game *game,GameStateEditor *games)
{
    //ctor
    this->games=games;
    this->game = game;
    sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
    this->view.setSize(pos);
    pos*=0.5f;
    this->view.setCenter(pos);
    this->gameStates=State::PAUSE;
    pos*=2.0f;
    this->guiSystem.emplace("layer",Gui(pos,0,false,this->game->styleSheets.at("layer"),
        {std::make_pair("","")}));
    pos*=0.5f;
    this->guiSystem.at("layer").setOrigin(pos);
    this->guiSystem.at("layer").setPosition(pos);
    this->guiSystem.at("layer").show();

    this->guiSystem.emplace("pausedState",Gui(sf::Vector2f(410,76),0,false,this->game->styleSheets.at("text2"),
        {std::make_pair("PAUSED","PAUSED")}));
    pos.y-=50;
    this->guiSystem.at("pausedState").setOrigin(sf::Vector2f(205,38));
    this->guiSystem.at("pausedState").setPosition(pos);
    this->guiSystem.at("pausedState").show();
    pos.y+=90;
    this->guiSystem.emplace("pausedMsg",Gui(sf::Vector2f(556,30),0,false,this->game->styleSheets.at("text2"),
        {std::make_pair("PRESS SPACE TO CONTINUE","pause_msg")}));
    this->guiSystem.at("pausedMsg").setOrigin(sf::Vector2f(278,15));
    this->guiSystem.at("pausedMsg").setPosition(pos);
    this->guiSystem.at("pausedMsg").show();

}

GameStatePause::~GameStatePause()
{
    //dtor
}

void GameStatePause::draw(const float dt)
{
    games->draw(dt);
    for(auto gui:this->guiSystem) this->game->window.draw(gui.second);

    return;
}

void GameStatePause::update(const float dt)
{

}

void GameStatePause::handleinput()
{
    sf::Event event;

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
                sf::Vector2f pos = sf::Vector2f(event.size.width,event.size.height);
                pos*=0.5f;
                pos=this->game->window.mapPixelToCoords(sf::Vector2i(pos),this->view);
                this->guiSystem.at("pausedState").setPosition(pos);
                //this->guiSystem.at("menu").setPosition(pos);
                this->game->background.setScale(
                    float(event.size.width)/float(this->game->background.getTexture()->getSize().x),
                    float(event.size.height)/float(this->game->background.getTexture()->getSize().y));
                break;
            }
            /* Key Activate */
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape) this->game->window.close();
                else if(event.key.code == sf::Keyboard::Space) this->backToGame();
                break;
            }
            default: break;
        }
    }

    return;
}

void GameStatePause::backToGame()
{
    this->game->popState();
}
