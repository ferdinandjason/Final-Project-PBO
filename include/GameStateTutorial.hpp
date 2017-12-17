#ifndef GAMESTATETUTORIAL_HPP
#define GAMESTATETUTORIAL_HPP

#include <GameState.hpp>


class GameStateTutorial : public GameState
{
    public:
        GameStateTutorial(Game *game);
        ~GameStateTutorial();

        virtual void draw(const float dt);
        virtual void update(const float dt);
        virtual void handleinput();

    protected:

    private:
        sf::View view;

        std::map<std::string,Gui> guiSystem;

        int counter=0;
};

#endif // GAMESTATETUTORIAL_HPP
