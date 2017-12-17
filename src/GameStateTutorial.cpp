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
    /*this->guiSystem.emplace("layer",Gui(sf::Vector2f(750,550),0,false,this->game->styleSheets.at("layer"),
        {std::make_pair("","")}));
    pos*=0.5f;
    this->guiSystem.at("layer").setOrigin(375,275);
    this->guiSystem.at("layer").setPosition(pos);
    this->guiSystem.at("layer").show();*/

    this->game->tutor1.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(15,25)));
    this->game->tutor1.setScale(1.1,1.1);
    this->game->tutor2.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(15,25)));
    this->game->tutor2.setScale(1.1,1.1);

    this->game->prev.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(60,40)));
    this->game->next.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(660,40)));
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

    if(this->counter==0) this->game->window.draw(this->game->tutor1);
    if(this->counter==1) this->game->window.draw(this->game->tutor2);

    this->game->window.draw(this->game->prev);
    this->game->window.draw(this->game->next);

    for(auto gui:this->guiSystem) this->game->window.draw(gui.second);

    return;
}

void GameStateTutorial::update(const float dt)
{
    if(this->counter<0) this->game->popState();
    if(this->counter>1) this->game->popState();

    sf::Event event;
    sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window),this->view);
    bool inNext=false;
    bool inPrev=false;

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
                this->guiSystem.at("menu").setPosition(pos);
                this->game->background.setScale(
                    float(event.size.width)/float(this->game->background.getTexture()->getSize().x),
                    float(event.size.height)/float(this->game->background.getTexture()->getSize().y));
                break;
            }
            /* Highlight menu items */
            case sf::Event::MouseMoved:
            {
                sf::Vector2f point = mousePos;
                point += this->game->prev.getOrigin();
                point -= this->game->prev.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->prev.getTexture()->getSize().x && point.y<=this->game->prev.getTexture()->getSize().y){
                    this->game->prev.setColor(sf::Color(0x94,0x94,0x94));
                }
                else{
                    this->game->prev.setColor(sf::Color(0xff,0xff,0xff));
                }

                point = mousePos;
                point += this->game->next.getOrigin();
                point -= this->game->next.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->next.getTexture()->getSize().x && point.y<=this->game->next.getTexture()->getSize().y){
                    this->game->next.setColor(sf::Color(0x94,0x94,0x94));
                }
                else{
                    this->game->next.setColor(sf::Color(0xff,0xff,0xff));
                }
                break;
            }
            /* Click on menu items. */
            case sf::Event::MouseButtonPressed:
            {
                sf::Vector2f point = mousePos;
                point += this->game->prev.getOrigin();
                point -= this->game->prev.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->prev.getTexture()->getSize().x && point.y<=this->game->prev.getTexture()->getSize().y) inPrev=true;
                else inPrev=false;

                if(inPrev)
                {
                    this->counter--;
                }

                point = mousePos;
                point += this->game->next.getOrigin();
                point -= this->game->next.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->next.getTexture()->getSize().x && point.y<=this->game->next.getTexture()->getSize().y) inNext=true;
                else inNext=false;

                if(inNext)
                {
                    this->counter++;
                }
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
