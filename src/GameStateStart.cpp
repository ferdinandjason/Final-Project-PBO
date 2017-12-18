#include <utility>

#include "GameStateStart.hpp"
#include "GameStateEditor.hpp"
#include "GameStateTutorial.hpp"

GameStateStart::GameStateStart(Game* game)
{
    //ctor
    this->game = game;
    sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
    this->view.setSize(pos);
    pos*=0.5f;
    this->view.setCenter(pos);
    this->gameStates=State::START;

//    this->logos.setTexture(this->te)

    /*
     * This Gui will act as a main menu for the game. To add new elements
     * to every othe std::map we've used the [] syntax, but we can't do
     * that here and must use emplace instead. This is because [] works by
     * first creating an empty object of the type you are trying to add
     * (thereby calling an empty constructor), then copying the object to
     * the right of the = into std::map. Gui doesn't have an empty constructor
     * so we use emplace, which works differently.
     *
     * We then place the Gui in the center of the screen before setting the Gui's
     * origin to its center. setOrigin uses local coordinates ((0,0) is the top
     * left of the Gui) unlike setPosition that uses world coordinates. Finally
     * we show the Gui to make it visible and place all the entries in the correct location.
     *
     */

    this->guiSystem.emplace("judul",Gui(sf::Vector2f(690,50),2,false,this->game->styleSheets.at("text3"),
        {std::make_pair("City Builder The Game","judul")}));
    //pos.x-=145;
    pos.y-=250;
    this->guiSystem.at("judul").setOrigin(sf::Vector2f(345,25));
    this->guiSystem.at("judul").setPosition(pos);
    this->guiSystem.at("judul").show();
    pos.y+=250;
    //pos.x+=145;
    this->guiSystem.emplace("menu",Gui(sf::Vector2f(192,32),4,false,game->styleSheets.at("button"),{std::make_pair(" Play Game!","load_game")}));
    pos.y+=150;
    this->guiSystem.at("menu").setPosition(pos);
    this->guiSystem.at("menu").setOrigin(96,16);
    //this->guiSystem.at("menu").show();



    sf::Vector2i koor=sf::Vector2i(159,3);

    sf::Vector2i origin(pos);
    int x=origin.x - this->game->play_button.getTexture()->getSize().x/2;
    int y=origin.y - this->game->play_button.getTexture()->getSize().y/2;
    this->game->play_button.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(x,y)));

    sf::Vector2i origin2(800,600);
    x=origin2.x - this->game->help_button.getTexture()->getSize().x;
    y=origin2.y - this->game->help_button.getTexture()->getSize().y;
    this->game->help_button.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(x,y)));

    //this->game->logos.setPosition(this->game->window.mapPixelToCoords(koor));

    this->backsound.openFromFile("music/backsoundStart.ogg");
    this->backsound.play();
    this->backsound.setLoop(true);
    this->backsound.setVolume(40);

}

GameStateStart::~GameStateStart()
{
    //dtor
}

void GameStateStart::draw(const float dt)
{
    this->game->window.setView(this->view);

    this->game->window.clear(sf::Color::Black);
    this->game->window.draw(this->game->backgroundStart);
    this->game->window.draw(this->game->play_button);
    this->game->window.draw(this->game->help_button);

    for(auto gui:this->guiSystem) this->game->window.draw(gui.second);

    return;
}

void GameStateStart::update(const float dt)
{

}

void GameStateStart::handleinput()
{
    sf::Event event;
    sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window),this->view);
    bool inPlayButton=false;
    bool inHelpButton=false;

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
                this->game->backgroundStart.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0,0)));


                sf::Vector2f pos = sf::Vector2f(event.size.width,event.size.height);
                pos*=0.5f;
                pos=this->game->window.mapPixelToCoords(sf::Vector2i(pos),this->view);
                this->guiSystem.at("menu").setPosition(pos);
                this->game->backgroundStart.setScale(
                    float(event.size.width)/float(this->game->backgroundStart.getTexture()->getSize().x),
                    float(event.size.height)/float(this->game->backgroundStart.getTexture()->getSize().y));

                pos = sf::Vector2f(event.size.width,event.size.height);
                pos.x -= this->game->help_button.getTexture()->getSize().x;
                pos.y -= this->game->help_button.getTexture()->getSize().y;
                this->game->help_button.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(pos.x,pos.y)));

                break;
            }
            /* Highlight menu items */
            case sf::Event::MouseMoved:
            {
                //this->guiSystem.at("menu").highlight(this->guiSystem.at("menu").getEntry(mousePos));

                sf::Vector2f point = mousePos;
                point += this->game->play_button.getOrigin();
                point -= this->game->play_button.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->play_button.getTexture()->getSize().x && point.y<=this->game->play_button.getTexture()->getSize().y){
                    this->game->play_button.setColor(sf::Color(0x94,0x94,0x94));
                }
                else{
                    this->game->play_button.setColor(sf::Color(0xff,0xff,0xff));
                }

                point = mousePos;
                point += this->game->help_button.getOrigin();
                point -= this->game->help_button.getPosition();
                if(point.x>=0 && point.y>=0 && point.x<=this->game->help_button.getTexture()->getSize().x && point.y<=this->game->help_button.getTexture()->getSize().y){
                    this->game->help_button.setColor(sf::Color(0x94,0x94,0x94));
                }
                else{
                    this->game->help_button.setColor(sf::Color(0xff,0xff,0xff));
                }

                break;
            }
            /* Click on menu items. */
            case sf::Event::MouseButtonPressed:
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f point = mousePos;
                    point += this->game->play_button.getOrigin();
                    point -= this->game->play_button.getPosition();
                    if(point.x>=0 && point.y>=0 && point.x<=this->game->play_button.getTexture()->getSize().x && point.y<=this->game->play_button.getTexture()->getSize().y) inPlayButton=true;
                    else inPlayButton=false;

                    if(inPlayButton)
                    {
                        this->loadGame();
                    }

                    point = mousePos;
                    point += this->game->help_button.getOrigin();
                    point -= this->game->help_button.getPosition();
                    if(point.x>=0 && point.y>=0 && point.x<=this->game->help_button.getTexture()->getSize().x && point.y<=this->game->help_button.getTexture()->getSize().y) inHelpButton=true;
                    else inHelpButton=false;

                    if(inHelpButton)
                    {
                        this->instruction();
                    }


                }
                break;
            }
            /* Key Activate */
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape) this->game->window.close();
                else if(event.key.code == sf::Keyboard::Space) this->loadGame();
                else if(event.key.code == sf::Keyboard::H) this->instruction();
                break;
            }
            default: break;
        }
    }

    return;
}

void GameStateStart::loadGame()
{
    this->backsound.stop();
    this->game->pushState(new GameStateEditor(this->game));

    return;
}

void GameStateStart::instruction()
{
    this->game->pushState(new GameStateTutorial(this->game));

    return;
}
