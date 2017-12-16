#include "FPS.hpp"

FPS::FPS()
{
    //ctor
}

FPS::~FPS()
{
    //dtor
}

float FPS::getFPS(const float dt,int Accelerator)
{
    return (1.0f / dt)*Accelerator;
}
