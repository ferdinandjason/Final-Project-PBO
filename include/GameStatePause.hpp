#ifndef GAMESTATEPAUSE_HPP
#define GAMESTATEPAUSE_HPP

#include <GameState.hpp>
#include "GameStateEditor.hpp"


class GameStatePause : public GameState
{
    public:
        GameStatePause(Game *game,GameStateEditor *games);
        ~GameStatePause();

        virtual void draw(const float dt);
        virtual void update(const float dt);
        virtual void handleinput();

    protected:

    private:
        GameStateEditor *games;
        sf::View view;

        std::map<std::string,Gui> guiSystem;

        void backToGame();
};

#endif // GAMESTATEPAUSE_HPP
