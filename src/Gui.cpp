#include "Gui.hpp"

Gui::~Gui()
{
    //dtor
}

/* returns the total dimensions of the Gui*/
sf::Vector2f Gui::getSize()
{
    return sf::Vector2f(this->dimensions.x, this->dimensions.y * this->entries.size());
}
/*
 * getEntry, which takes the mouse position (in screen coordinates, or really the coordinates
 * for the sf::View that the Gui is displayed on) as an argument and returns the index of
 * the entry that mouse is hovering over. If the mouse is outside of the Gui then it returns -1.
 *
 * By adding the origin of the entry's shape to the mouse position and subtracting the position
 * of the shape we convert point from view coordinates to 'local' coordinates, where (0,0) is
 * the top left of the GuiEntry in question and the coordinates extend up to its dimensions
 */
int Gui::getEntry(const sf::Vector2f mousePos)
{
    if(entries.size() == 0) return -1;
    if(!this->visible) return -1;
    for(int i = 0; i < this->entries.size(); ++i)
    {
        sf::Vector2f point = mousePos;
        point += this->entries[i].shape.getOrigin();
        point -= this->entries[i].shape.getPosition();

        if(point.x < 0 || point.x > this->entries[i].shape.getScale().x*this->dimensions.x) continue;
        if(point.y < 0 || point.y > this->entries[i].shape.getScale().y*this->dimensions.y) continue;
        return i;
    }

    return -1;
}
/* setEntryText, which of course takes an std::string as an argument and sets the text of the specified entry accordingly. */
void Gui::setEntryText(int entry, std::string text)
{
    if(entry >= entries.size() || entry < 0) return;
    entries[entry].text.setString(text);
    return;
}
/* setDimensions simply changes the size of all the entries */
void Gui::setDimensions(sf::Vector2f dimensions)
{
    this->dimensions = dimensions;
    for(auto& entry : entries)
    {
        entry.shape.setSize(dimensions);
        entry.text.setCharacterSize(dimensions.y-style.borderSize-padding);
    }
    return;
}
/* Now draw is far more interesting. We are overriding the pure virtual function draw in the sf::Drawable class that Gui inherits from, and by doing this we can use the window.draw(gui) */
void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if(!visible) return;
    for(auto entry : this->entries)
    {
        target.draw(entry.shape);
        target.draw(entry.text);
    }
    return;
}

/*
 * The show and hide functions change the visibility of the Gui; if it isn't visible, it won't be drawn.
 * show does more than that however, and is used to calculate the position that each GuiEntry
 * should be in; we could put that in draw but we aren't allowed to because of the const!
 */

void Gui::show()
{
    sf::Vector2f offset(0.0f, 0.0f);
    this->visible = true;
    for(auto& entry : this->entries)
    {
        sf::Vector2f origin = this->getOrigin();
        origin -= offset;
        entry.shape.setOrigin(origin);
        entry.text.setOrigin(origin);
        /* Compute the position of the entry. */
        entry.shape.setPosition(this->getPosition());
        entry.text.setPosition(this->getPosition());

        if(this->horizontal) offset.x += this->dimensions.x;
        else offset.y += this->dimensions.y;
    }
    return;
}

void Gui::hide()
{
    this->visible = false;
    return;
}

/*
 * Highlight and activate functions. highlight simply changes the colors of each entry to use the
 * highlighted or normal versions from the GuiStyle depending on whether they are marked as
 * highlighted or not, and activate returns the message associated with the entry. The second
 * activate combines the first activate and getEntry together into a more handy function.
 */

int Gui::highlight(const int entry)
{
    bool selected=false;
    for(int i = 0; i < entries.size(); ++i)
    {
        if(i == entry)
        {
            entries[i].shape.setFillColor(style.bodyHighlightCol);
            entries[i].shape.setOutlineColor(style.borderHighlightCol);
            entries[i].text.setColor(style.textHighlightCol);
            selected=true;
        }
        else
        {
            entries[i].shape.setFillColor(style.bodyCol);
            entries[i].shape.setOutlineColor(style.borderCol);
            entries[i].text.setColor(style.textCol);
        }
    }
    return selected;
}

/* Return the message bound to the entry. */
std::string Gui::activate(const int entry)
{
    if(entry == -1) return "null";
    return entries[entry].message;
}

std::string Gui::activate(const sf::Vector2f mousePos)
{
    int entry = this->getEntry(mousePos);
    return this->activate(entry);
}
