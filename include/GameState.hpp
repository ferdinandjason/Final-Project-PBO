#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "Game.hpp"

/*
 * GameState class : (Pure Virtual Class)
 * Which our actual states will inherit from, and then creating a Game class
 * that will handle the changing of the states as well as storing the information
 * that is universal to (almost) every state
 *
 * The compiler wouldn't allow you to create an instance of this class
 *
 * Our game will use three states, GameStateStart, GameStateEditor and GameStatePaused
 */

 enum class State {START,PLAY,PAUSE};

class GameState
{
    public:
        Game *game;

        virtual void draw(const float dt) = 0;
        virtual void update(const float dt) = 0;
        virtual void handleinput() = 0;

        State gameStates;

    protected:

    private:
};

#endif // GAMESTATE_HPP
