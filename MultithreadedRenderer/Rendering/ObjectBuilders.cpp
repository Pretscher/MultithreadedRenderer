#include "ObjectBuilders.hpp"

//Text Builder---------------------------------------------------------------------------------------------------------------------------------------------------

TextBuilder& TextBuilder::centerToRect(int rectX, int rectY, int rectWidth, int rectHeight) {
    std::string temp = text->getString().toAnsiString();
    bool bold = (text->getStyle() == sf::Text::Bold);

    int maxHeight = 0;
    for (int x = 0; x < text->getString().getSize(); x++)
    {
        sf::Uint32 Character = temp.at(x);
        const sf::Glyph& CurrentGlyph = text->getFont()->getGlyph(Character, text->getCharacterSize(), bold);

        int Height = CurrentGlyph.bounds.height;

        if (maxHeight < Height) {
            maxHeight = Height;
        }
    }

    sf::FloatRect rect = text->getGlobalBounds();

    rect.left = ((float)rectWidth / 2) - (rect.width / 2);
    rect.top = ((float)rectHeight / 2) - ((float)maxHeight / 2) - (rect.height - maxHeight) + (((float)rect.height - (text->getCharacterSize() * 1.5))) / 2;

    text->setPosition(rect.left + rectX, rect.top + rectY);
    return *this;
}

std::map<std::string, sf::Font*> TextBuilder::loadedFonts;//we don't need to delete the fonts because this static map will persist until the program ends.
std::optional<sf::Font*> TextBuilder::loadFont(std::string fontName) {
    if (TextBuilder::loadedFonts.count(fontName) == 0) {//if font not already in set, load it
        sf::Font* font = new sf::Font();
        const std::string path = "myRecources/Fonts/" + fontName + ".ttf";
        if (!font->loadFromFile(path)) {
            std::cout << "ERROR loading font with path " << path << " coult not be loaded. Text will not be displayed. \n";
            //DO NOT select a font if there was an error loading, will cause in crash when trying to draw the text
            return {};
        }
        else {
            TextBuilder::loadedFonts[fontName] = font;
            return TextBuilder::loadedFonts[fontName];//retrieve from map so that the reference persists
        }
    }
    else {
        return TextBuilder::loadedFonts[fontName];
    }
}
