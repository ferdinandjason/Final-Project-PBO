#ifndef GAMESTATEEDITOR_HPP
#define GAMESTATEEDITOR_HPP

#include <SFML/Graphics.hpp>

#include "GameState.hpp"
#include "GameStateEditor.hpp"
#include "Map.hpp"
#include "City.hpp"

/*
 * GameStateEditor class: (Game Class)
 * Ovveridden pure virtual class in the base class,
 * pointer into Game
 *
 * sf::View variable is a lot like a camera and the windows display what all the cameras all seeing
 *
 * window.setView() changes the current view that will be draw to window
 * draw() draw its argunment to the window current view (background)
 *
 * pollEvent function return true or false of the event
 * Event :
 * - Closed = user trying to close the window, goto close function
 * - Resized = when a window is resized, the view will continue to look at the same part of the game world will change its size
 *      a. mapPixelToCoords : convert a position in window coordinates to its equivalent position in world coordinates using current view
 *      b. setPosition      : requires an sf::Vector2f
 *      c. setScale         : scaling background
 * - KeyPressed = which key was pressed, and if esc key pressed then close the window
 *
 *
 * enum class definition again to create the ActionState type; if actionState == ActionState::PANNING
 *
 */

 enum class ActionState {NONE,PANNING,SELECTING};


class GameStateEditor : public GameState
{
    public:
        GameStateEditor(Game* game);
        ~GameStateEditor();

        virtual void draw(const float dt);
        virtual void update(const float dt);
        virtual void handleinput();

    protected:

    private:
        ActionState actionState;

        sf::View gameView;
        sf::View guiView;

        City city;

        sf::Vector2i panningAnchor;
        float zoomLevel;

        sf::Vector2i selectionStart;
        sf::Vector2i selectionEnd;

        std::map<std::string, Gui> guiSystem;

        Tile* currentTile;

};

#endif // GAMESTATEEDITOR_HPP
