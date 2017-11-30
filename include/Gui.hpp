#ifndef GUI_HPP
#define GUI_HPP

#include <vector>
#include <utility>
#include <string>

#include <SFML/Graphics.hpp>

/*
 * First up is the Gui class, which we'll use to draw (as you might expect) a graphical user interface.
 * We could use a separate library dedicated to this task, but the task is simple enough (in this case)
 * for us to roll our own. Actually, there are 4 classes, because gui.hpp contains 3; GuiStyle, GuiEntry,
 * and Gui. A Gui is made up of a series of GuiEntrys which each display some text and are styled according to a GuiStyle.
 */


 /*
  * GuiStyle : (Gui Sub Class)
  * sf::Fonts are large things, and so should only be created once, like sf::Textures.
  * As a quick overview of the GuiStyle, bodyCol is the background color of the entry,
  * borderCol is the color of the entry's outline, textCol is the color of the text
  * written on the entry,
  */

class GuiStyle{
    public:

    sf::Color bodyCol;
    sf::Color bodyHighlightCol;
    sf::Color borderCol;
    sf::Color borderHighlightCol;
    sf::Color textCol;
    sf::Color textHighlightCol;
    sf::Font* font;

    float borderSize;

    GuiStyle(sf::Font* font, float borderSize,sf::Color bodyCol, sf::Color borderCol, sf::Color textCol,sf::Color bodyHighlightCol, sf::Color borderHighlightCol, sf::Color textHighlightCol)
    {
        this->bodyCol = bodyCol;
        this->borderCol = borderCol;
        this->textCol = textCol;
        this->bodyHighlightCol = bodyHighlightCol;
        this->borderHighlightCol = borderHighlightCol;
        this->textHighlightCol = textHighlightCol;
        this->font = font;
        this->borderSize = borderSize;
    }
    GuiStyle() { }
};

/*
 * GuEntry Class : (Gui Subclass)
 * sf:rectangleShape shape      Handles appearance of the entry
 * std::string message          String returned when the entry is activated
 * sf::Text text                Text displayed on the entry
 */


class GuiEntry
{
    public:
        sf::RectangleShape shape;
        std::string message;
        sf::Text text;

        GuiEntry(const std::string& message, sf::RectangleShape shape, sf::Text text)
        {
            this->message = message;
            this->shape = shape;
            this->text = text;
        }
        GuiEntry() { }
};

/*
 * Gui Class : (Gui Main Class)
 *
 * sf::Vector2f getSize     getSize
 * int getEntry             Return the entry that the mouse is hovering over. Returns -1 if the mouse if outside of the Gui
 * void setEntryText        Change the text of an entry.
 * void setDimensions       Change the entry dimensions.
 * virtual void draw        Draw the menu
 * void show
 * void hide
 * void higlight            Highlights an entry of the menu
 * std::string activate     Return the message bound to the entry.
 *
 */


class Gui : public sf::Transformable,public sf::Drawable
{
    public:
        Gui(sf::Vector2f dimensions, int padding, bool horizontal,GuiStyle& style, std::vector<std::pair<std::string, std::string>> entries)
        {
            visible = false;
            this->horizontal = horizontal;
            this->style = style;
            this->dimensions = dimensions;
            this->padding = padding;
            /* Construct the background shape */
            sf::RectangleShape shape;
            shape.setSize(dimensions);
            shape.setFillColor(style.bodyCol);
            shape.setOutlineThickness(-style.borderSize);
            shape.setOutlineColor(style.borderCol);
            /* Construct each gui entry */
            for(auto entry : entries)
            {
                sf::Text text;
                text.setString(entry.first);
                text.setFont(*style.font);
                text.setColor(style.textCol);
                text.setCharacterSize(dimensions.y-style.borderSize-padding);

                this->entries.push_back(GuiEntry(entry.second, shape, text));
            }
        };
        ~Gui();
        sf::Vector2f getSize();
        int getEntry(const sf::Vector2f mousePos);
        void setEntryText(int entry, std::string text);
        void setDimensions(sf::Vector2f dimensions);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void show();
        void hide();
        void highlight(const int entry);
        std::string activate(const int entry);
        std::string activate(const sf::Vector2f mousePos);


        std::vector<GuiEntry> entries;
        bool visible;

    protected:

    private:
        bool horizontal;
        GuiStyle style;
        sf::Vector2f dimensions;
        int padding;
};

/*
 * Gui inherits from two SFML classes, sf::Transformable and sf::Drawable.
 * These allow us to move the Gui around (like we can a sprite) and also use t
 * he window.draw instead of draw(window) syntax. As arguments the constructors takes
 * the dimensions of each GuiEntry, the padding (in pixels) that surrounds the text
 * to stop it from overlapping the edges, whether the Gui should arrange the entries
 * horizontally or vertically, which GuiStyle it should use, and an std::vector that
 * contains a pair of std::strings, where the first is the text of the entry and the s
 * second is the message. The argument itself might look ugly but passing values to it isn't too bad.
 *
 * We then set the variables accordingly and create the sf::RectangleShape that will be used on
 * each GuiEntry. The functions describe themselves but one thing to note is the - in the
 * setOutlineThickness function. This is so that the outline expands inwards toward the center of
 * the shape instead of outwards, preserving the size of the shape and stopping the borders of
 * adjacent elements from overlapping.
 *
 * We then iterate over each entry pair and create the text using the arguments and the GuiStyle.
 * Here setCharacterSize is the function of note; so that the text fits inside the entry
 * correctly we subtract the borderSize and the padding from the height of the entry shape
 */

#endif // GUI_HPP
