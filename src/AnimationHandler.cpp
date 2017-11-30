#include "AnimationHandler.hpp"

AnimationHandler::AnimationHandler()
{
    //ctor
    this->t = 0.0f;
    this->currentAnim=-1;
}

AnimationHandler::AnimationHandler(const sf::IntRect& frameSize)
{
    this->frameSize = frameSize;
    this->t = 0.0f;

    this->currentAnim = -1;
}

/*
 * addAnimations just adds the specified animation to the animtion
 */

void AnimationHandler::addAnimation(Animation& anim)
{
    this->animations.push_back(anim);
}

/*
 * Checking animations has progressed to a new frame and if so change to next frames
 * -- calculate the frame number,adjust for looping,set the sprite to the new frame
 *
 * We dont update if the current animation does not exist
 *
 */

void AnimationHandler::update(const float dt)
{
    if(currentAnim >= this->animations.size() || currentAnim<0 ) return;
    float duration = this->animations[currentAnim].duration;

    if(int((t + dt)/duration) > int(t/duration))
    {
        int frame = int((t + dt)/duration);
        //printf("%d %f %f %f\n",frame,t,dt,duration);
        frame%=this->animations[currentAnim].getLength();
        sf::IntRect rect = this->frameSize;
        rect.left = rect.width * frame;
        rect.top = rect.height * this->currentAnim;
        this->bounds = rect;
    }
    this->t+=dt;
    if(this->t > duration * this->animations[currentAnim].getLength())
    {
        this->t = 0.0f;
    }
}

/*
 * set current animation to the new one (if the new one is valid)
 * Do not change the animation if the animations is currently active or the new animations does not exist
 */

void AnimationHandler::changeAnimation(unsigned int animID)
{
    if(this->currentAnim==animID || animID >= this->animations.size() || animID < 0) return;

    this->currentAnim=animID;

    sf::IntRect rect = this->frameSize;
    rect.top=rect.height * animID;
    this->bounds=rect;
    this->t=0.0;

    return;
}


AnimationHandler::~AnimationHandler()
{
    //dtor
}
