#include <map>
#include <string>

#include "GameStateEditor.hpp"
#include "Gui.hpp"

GameStateEditor::GameStateEditor(Game* game)
{
    //ctor
    this->game = game;
    sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
    this->guiView.setSize(pos);
    this->gameView.setSize(pos);
    pos.x*=0.5;
    pos.y*=0.5;
    this->guiView.setCenter(pos);
    this->gameView.setCenter(pos);

    this->city = City("city", this->game->tileSize, this->game->tileAtlas);
    this->city.shuffleTiles();

    /* Create gui elements. */
    this->guiSystem.emplace("rightClickMenu", Gui(sf::Vector2f(196, 16), 2, false, this->game->styleSheets.at("button"),
        {
            std::make_pair("Flatten $"          + this->game->tileAtlas["grass"].getCost(),         "grass"),
            std::make_pair("Forest $"           + this->game->tileAtlas["forest"].getCost(),        "forest" ),
            std::make_pair("Residential Zone $" + this->game->tileAtlas["residential"].getCost(),   "residential"),
            std::make_pair("Commercial Zone $"  + this->game->tileAtlas["commercial"].getCost(),    "commercial"),
            std::make_pair("Industrial Zone $"  + this->game->tileAtlas["industrial"].getCost(),    "industrial"),
            std::make_pair("Road $"             + this->game->tileAtlas["road"].getCost(),          "road")
        }));

    this->guiSystem.emplace("selectionCostText", Gui(sf::Vector2f(196, 16), 0, false, this->game->styleSheets.at("text"),
        { std::make_pair("", "") }));

    this->guiSystem.emplace("infoBar", Gui(sf::Vector2f(this->game->window.getSize().x / 5 , 16), 2, true, this->game->styleSheets.at("button"),
        {
            std::make_pair("time",          "time"),
            std::make_pair("funds",         "funds"),
            std::make_pair("population",    "population"),
            std::make_pair("employment",    "employment"),
            std::make_pair("current tile",  "tile")
        }));
    this->guiSystem.at("infoBar").setPosition(sf::Vector2f(0, this->game->window.getSize().y - 16));
    this->guiSystem.at("infoBar").show();

    this->zoomLevel = 1.0f;

    /* Centre the caera on the map */
    sf::Vector2f centre(this->city.map.width,this->city.map.height*0.5);
    centre*=float(this->city.map.tileSize);
    gameView.setCenter(centre);

    this->selectionStart = sf::Vector2i(0, 0);
    this->selectionEnd = sf::Vector2i(0, 0);

    this->currentTile = &this->game->tileAtlas.at("grass");
    this->actionState=ActionState::NONE;


}

GameStateEditor::~GameStateEditor()
{
    //dtor
}

/*
 * We want the background to always be drawn in the same place, so we draw it on
 * guiView, but the world is part of the game and so should be drawn to gameView.
 * If you compile the code now you should see a nicely centred Map being displayed
 * in front of background, which should expand as you resize the window whilst the
 * Map stays in the same (relative) place.
 *
 * First, we switch to the guiView and then we draw each Gui in turn like in
 * GameStateStart. The Gui is of course drawn after the city, otherwise the
 * player wouldn't be able to see it! Finally we can add the Gui code to handleInput.
 * Before we get to the switch statement it would be sensible to save ourselves some w
 * riting and create gamePos and guiPos variables that record the position of t
 * he mouse in gameView and guiView coordinates.
 */

void GameStateEditor::draw(const float dt)
{
    this->game->window.clear(sf::Color::Black);

    this->game->window.setView(this->guiView);
    this->game->window.draw(this->game->background);

    this->game->window.setView(this->gameView);
    this->city.map.draw(this->game->window, dt);

    this->game->window.setView(this->guiView);
    for(auto gui : this->guiSystem) this->game->window.draw(gui.second);

    return;
}

/*
 * The "rightClickMenu" will (as its name implies) be shown when the player presses
 * the right mouse button. It will list all of the possible tiles that they can place
 * along with their prices, and when a tile is chosen currentTile will be set to that
 * tile. Any time the player selects tiles, from then on, will cause the selected
 * tiles to be replaced with currentTile. "selectionCostText" will be displayed when
 * the player is selecting tiles, and will tell them how much the tiles they are placing
 * will cost. It will go red if the player does not have enough funds to place the tiles.
 *
 * Lastly, "infoBar" will sit and span the bottom of the screen, displaying the game day
 * and other useful information for the player. If we want it to display information we'll
 * have to update that information all the time, so we can put that code inside of update
 *
 * The first entry of the "infobar" will be the game day, the second the city's funds,
 * the third the population (with the number of homeless in parentheses), the fourth the
 * number of employable people (with the number left unemployed in parentheses), and the
 * fifth the name of the currentTile. We have to first typecast to a long so that we don't
 * get floating point populations appearing (we used a double for ease of calculation but
 * we truncate it here to print the 'real' value). With update done let's make sure that
 * the Gui is actually drawn to the screen.
 */

void GameStateEditor::update(const float dt)
{
    this->city.update(dt);

    /* Update the info bar at the bottom of the screen */
    this->guiSystem.at("infoBar").setEntryText(0, "Day: " + std::to_string(this->city.day));
    this->guiSystem.at("infoBar").setEntryText(1, "$" + std::to_string(long(this->city.funds)));
    this->guiSystem.at("infoBar").setEntryText(2, std::to_string(long(this->city.population)) + " (" + std::to_string(long(this->city.getHomeless())) + ")");
    this->guiSystem.at("infoBar").setEntryText(3, std::to_string(long(this->city.employable)) + " (" + std::to_string(long(this->city.getUnemployed())) + ")");
    this->guiSystem.at("infoBar").setEntryText(4, tileTypeToStr(currentTile->tileType));

    /* Highlight entries of the right click context menu */
    this->guiSystem.at("rightClickMenu").highlight(this->guiSystem.at("rightClickMenu").getEntry(this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->guiView)));

    return;
}

void GameStateEditor::handleinput()
{
    sf::Event event;

    sf::Vector2f guiPos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->guiView);
    sf::Vector2f gamePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView);

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
                gameView.setSize(event.size.width, event.size.height);
                gameView.zoom(zoomLevel);
                guiView.setSize(event.size.width,event.size.height);
                this->guiSystem.at("infoBar").setDimensions(sf::Vector2f(event.size.width / this->guiSystem.at("infoBar").entries.size(), 16));
                this->guiSystem.at("infoBar").setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0, event.size.height - 16), this->guiView));
                this->guiSystem.at("infoBar").show();
                this->game->background.setPosition(this->game->window.mapPixelToCoords(sf::Vector2i(0,0),this->guiView));
                this->game->background.setScale(
                    float(event.size.width)/float(this->game->background.getTexture()->getSize().x),
                    float(event.size.height)/float(this->game->background.getTexture()->getSize().y));
                break;
            }
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::Escape) this->game->window.close();
                break;
            }

            case sf::Event::MouseMoved:
            {
                /* Pan the camera */
                if(this->actionState == ActionState::PANNING)
                {
                    sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(this->game->window) - this->panningAnchor);
                    gameView.move(-1.0f * pos * this->zoomLevel);
                    panningAnchor = sf::Mouse::getPosition(this->game->window);
                } /* Select Tiles */
                else if(actionState == ActionState::SELECTING)
                {
                    sf::Vector2f pos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView);
                    selectionEnd.x = pos.y/(this->city.map.tileSize) + pos.x/(2*this->city.map.tileSize) - this->city.map.width *0.5-0.5;
                    selectionEnd.y = pos.y/(this->city.map.tileSize) - pos.x/(2*this->city.map.tileSize) + this->city.map.width *0.5+0.5;
                    printf("%d %d , %.0lf %.0lf\n",selectionEnd.x,selectionEnd.y,pos.x,pos.y);


                    this->city.map.clearSelected();
                    if(this->currentTile->tileType == TileType::GRASS)
                    {
                        this->city.map.select(selectionStart, selectionEnd, {this->currentTile->tileType, TileType::WATER});
                    }
                    else
                    {
                        this->city.map.select(selectionStart, selectionEnd,
                            {
                                this->currentTile->tileType,    TileType::FOREST,
                                TileType::WATER,                TileType::ROAD,
                                TileType::RESIDENTIAL,          TileType::COMMERCIAL,
                                TileType::INDUSTRIAL
                            });
                    }
                    /* Selection Cost Text */
                    this->guiSystem.at("selectionCostText").setEntryText(0, "$" + std::to_string(this->currentTile->cost * this->city.map.numSelected));
                    if(this->city.funds <= this->city.map.numSelected * this->currentTile->cost)
                        this->guiSystem.at("selectionCostText").highlight(0);
                    else
                        this->guiSystem.at("selectionCostText").highlight(-1);
                    this->guiSystem.at("selectionCostText").setPosition(guiPos + sf::Vector2f(16, -16));
                    this->guiSystem.at("selectionCostText").show();
                    }
                    /* Highlight entries of the right click context menu */
                    this->guiSystem.at("rightClickMenu").highlight(this->guiSystem.at("rightClickMenu").getEntry(guiPos));
                }
                break;
            case sf::Event::MouseButtonPressed:
            {
                /* Start panning */
                if(event.mouseButton.button == sf::Mouse::Middle)
                {
                    if(this->actionState != ActionState::PANNING)
                    {
                        this->guiSystem.at("rightClickMenu").hide();
                        this->guiSystem.at("selectionCostText").hide();
                        this->actionState = ActionState::PANNING;
                        this->panningAnchor = sf::Mouse::getPosition(this->game->window);
                    }
                }
                else if(event.mouseButton.button == sf::Mouse::Left)
                {
                    /* Select a context menu entry. */
                    if(this->guiSystem.at("rightClickMenu").visible == true)
                    {
                        std::string msg = this->guiSystem.at("rightClickMenu").activate(guiPos);
                        if(msg != "null") this->currentTile = &this->game->tileAtlas.at(msg);

                        this->guiSystem.at("rightClickMenu").hide();
                    }
                    /* Select map tile. */
                    else
                    {
                        /* Select map tile */
                        if(this->actionState != ActionState::SELECTING)
                        {
                            this->actionState = ActionState::SELECTING;
                            sf::Vector2f pos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->gameView);
                            selectionStart.x = pos.y / (this->city.map.tileSize) + pos.x / (2*this->city.map.tileSize) - this->city.map.width * 0.5 - 0.5;
                            selectionStart.y = pos.y / (this->city.map.tileSize) - pos.x / (2*this->city.map.tileSize) + this->city.map.width * 0.5 + 0.5;
                        }
                    }
                }
                else if(event.mouseButton.button == sf::Mouse::Right)
                {
                    /* Stop selecting */
                    if(this->actionState == ActionState::SELECTING)
                    {
                        this->actionState = ActionState::NONE;
                        this->city.map.clearSelected();
                    }
                    else
                    {
                        /* Open the tile select menu. */
                        sf::Vector2f pos = guiPos;
                        if(pos.x > this->game->window.getSize().x - this->guiSystem.at("rightClickMenu").getSize().x)
                        {
                            pos -= sf::Vector2f(this->guiSystem.at("rightClickMenu").getSize().x, 0);
                        }
                        if(pos.y > this->game->window.getSize().y - this->guiSystem.at("rightClickMenu").getSize().y)
                        {
                            pos -= sf::Vector2f(0, this->guiSystem.at("rightClickMenu").getSize().y);
                        }
                        this->guiSystem.at("rightClickMenu").setPosition(pos);
                        this->guiSystem.at("rightClickMenu").show();
                    }
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                /* Stop panning*/
                if(event.mouseButton.button == sf::Mouse::Middle)
                {
                    this->actionState = ActionState::NONE;
                } /* Stop Selecting */
                else if(event.mouseButton.button == sf::Mouse::Left)
                {
                    if(this->actionState == ActionState::SELECTING)
                    {
                        /* Replace tiles if enough funds and a tile is selected */
                        if(this->currentTile != nullptr)
                        {
                            unsigned int cost = this->currentTile->cost * this->city.map.numSelected;
                            if(this->city.funds >= cost)
                            {
                                this->city.bulldoze(*this->currentTile);
                                this->city.funds -= this->currentTile->cost * this->city.map.numSelected;
                                this->city.tileChanged();
                            }
                        }
                        this->guiSystem.at("selectionCostText").hide();
                        this->actionState = ActionState::NONE;
                        this->city.map.clearSelected();
                    }
                }
                break;
            }
            case sf::Event::MouseWheelMoved:
            {
                /* Zoom the view */
                if(event.mouseWheel.delta < 0)
                {
                    gameView.zoom(1.25f);
                    zoomLevel *= 1.25f;
                }
                else
                {
                    gameView.zoom(0.75f);
                    zoomLevel *= 0.75f;
                }
                break;
            }
            default: break;
        }
    }

    return;
}

/*
 * When the middle mouse button is pressed and the player is not already panning the camera (this is why we created actionState)
 * we set the panningAnchor to the position of the mouse. This is a screen position, and has nothing to do with the views we created.
 * We also set actionState so that the program knows that the player is panning.
 *
 * When the middle mouse button is released, we set the actionState to ActionState::NONE so that the player is not panning anymore.
 * If the mouse moves whilst the player is panning then we get the new position of the mouse and subtract the old position
 * (the panningAnchor) from it. Since both positions are coordinates, we can interpet this as calculating the (mathematical)
 * vector from the anchor to the mouse. We then move the gameView in the direction that vector points.
 */
