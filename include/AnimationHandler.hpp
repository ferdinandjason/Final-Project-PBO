#ifndef ANIMATIONHANDLER_HPP
#define ANIMATIONHANDLER_HPP

#include <SFML/Graphics.hpp>
#include <vector>

/*
 * Animation and AnimationHandler Class : (Animation Class)
 *
 * Each sprite will have its own texture file that will contain all the
 * animation stages for its different animations. It would be more efficient
 * (for the computer, not us!) to store multiple sprites in the same file,
 * but that makes the code and asset creation more complex so we won't do
 * that. Anyway, we'll split the texture file into a grid, where each frame
 * of the same animation extends to the right, and each animation extends
 * downwards.
 *
 */

class Animation
{
    public:
        unsigned int startFrame;
        unsigned int endFrame;

        float duration;

        Animation(unsigned int startFrame,unsigned int endFrame,float duration)
        {
            this->startFrame = startFrame;
            this->endFrame = endFrame;
            this->duration = duration;
        }

        unsigned int getLength() {return endFrame-startFrame+1;}
};
/*
 * AnimationHandler class :
 *
 * std::vector<Animation> animations    make a array of animations
 * float t                              Current time since the animation loop started;
 * int currentAnim                      current animation
 *
 * addAnimation() -> add a new animation
 * update() -> update the current frame of animation
 * changeAnimation() -> change the animation, resetting t in the process
 *
 * sf::IntRect bounds       current section of the texture that should be displayed
 * sf::IntRect frameSize    pixel dimensions of each individual frame
 */

class AnimationHandler
{
    public:
        AnimationHandler();
        AnimationHandler(const sf::IntRect& frameSize);
        ~AnimationHandler();
        void addAnimation(Animation& anim);
        void update(const float dt);
        void changeAnimation(unsigned int animID);

        sf::IntRect bounds;
        sf::IntRect frameSize;
;
    protected:

    private:
        std::vector<Animation> animations;
        float t;
        int currentAnim;
};

#endif // ANIMATIONHANDLER_HPP
