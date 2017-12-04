#include "FPS.hpp"

FPS::FPS()
{
    //ctor
}

FPS::~FPS()
{
    //dtor
}

float FPS::getFPS(const float dt)
{
    return (1.0f / dt);
}
