#ifndef GAMESTATESTART_HPP
#define GAMESTATESTART_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "GameState.hpp"

/*
 * GameStateStart class: (Main Menu Class)
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
 * loadGame() private function that activated to push newGameState on to the stack GameState
 * activated by pressing space!
 *
 * guiSystem -> class to create Gui System in Game Start
 */

class GameStateStart : public GameState
{
    public:
        GameStateStart(Game* game);
        ~GameStateStart();

        virtual void draw(const float dt);
        virtual void update(const float dt);
        virtual void handleinput();

        sf::Music backsound;

    protected:

    private:
        sf::View view;

        std::map<std::string,Gui> guiSystem;

        void loadGame();

};

#endif // GAMESTATESTART_HPP
