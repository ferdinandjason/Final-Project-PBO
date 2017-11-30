#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <string>
#include <map>

#include <SFML/Graphics.hpp>

/*
 * TextureManager class :(Store Texture)
 * Store the texture whole the game
 *
 * loadTexture  : load textures and add into the list of textures
 * getRef       : getreferences from the Texture whose name is passed into a the function
 *  - using at because provides bounds checkk                                                                                                                                                                                                                                                           ing and will throw an exception if the specified element does not exist
 */

class TextureManager
{
    public:
        TextureManager();
        ~TextureManager();

        void loadTexture(const std::string& name,const std::string &filename); // Add a texture from a file
        sf::Texture& getRef(const std::string &texture);                       // Translate an id into a reference

    protected:

    private:
        std::map<std::string,sf::Texture> textures;                            // Array of Textures used
};

#endif // TEXTUREMANAGER_HPP
