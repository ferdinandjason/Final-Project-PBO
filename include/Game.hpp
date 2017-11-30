#ifndef GAME_HPP
#define GAME_HPP

#include <stack>
#include <map>
#include <string>

#include <SFML/Graphics.hpp>

#include "TextureManager.hpp"
#include "Tile.hpp"
#include "Gui.hpp"

/*
 * Game Class: (Main Class)
 * pushState   : takes pointer to a GameState and pushes it on to the state stack
 * popState    : remove the top state from the stack
 * changeState : pops the previous state and then pushes the new one on
 * peekState   : return pointer to whatever state is on top of the stack
 *
 * sf::RenderWindow window which 800 pixel wide and 600 pixel high
 * cap the framerate to 60fps, to stop the game from running to fast and eating too much cpu cycles
 * TextureManager, store the texture
 * sf::Sprite background,
 *
 * tileAtlas map string to tile
 * sytlesheets to store the styles, add also loadStyleSheets
 * fonts to store the fonts, add also loadFonts
 *
 *
 * run() function :
 * - "int main()" to the project
 * - using timestep called dt variable
 * - clock.restart return the time since clock started
 */

class GameState;

class Game
{
    public:
        Game();
        ~Game();

        const static int tileSize = 8;

        std::stack<GameState*> states;

        sf::RenderWindow window;
        TextureManager texmgr;
        sf::Sprite background;

        std::map<std::string,Tile> tileAtlas;
        std::map<std::string,GuiStyle> styleSheets;
        std::map<std::string,sf::Font> fonts;

        void pushState(GameState* state);
        void popState();
        void changeState(GameState* state);
        GameState* peekState();

        void run();

    protected:

    private:
        void loadTextures();
        void loadTiles();
        void loadSytleSheets();
        void loadFonts();
};

#endif // GAME_HPP
