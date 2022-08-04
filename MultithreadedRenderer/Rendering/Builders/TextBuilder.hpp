#pragma once
#include <optional>

class TextBuilder {
protected:
    sf::Text* text;
    Renderer* renderer;
    bool fontLoaded = false;
public:
    /**
     * @brief Construct a new Text Builder object. If the font for the text can't be loaded, the built text will be a nullptr. 
     * 
     * @param renderer used for adding the text to the renderer's permanent objects or drawing them once.
     * @param x 
     * @param y 
     */
    TextBuilder(Renderer* renderer, int x, int y) {
        text = new sf::Text();
        this->renderer = renderer;
        text->setPosition(x, y);

        //default values
        setFont("calibri");
    }
    /**
     * @brief Permanently draw this text. 
     * 
     * @return sf::Drawable* 
     */
    virtual sf::Text* build() {
        if(fontLoaded == false) {
            std::cout << "can't build or get text without a font";
            return nullptr;
        }
        renderer->addPermanentObject(text);
        return text;
    }
    /**
     * @brief Draw this shape once and call Renderer::draw() with the pointer to draw another time.
     * 
     * @return sf::Drawable* 
     */
    virtual sf::Text* drawOnce() {
        if(fontLoaded == false) {
            std::cout << "can't draw or get text without a font";
            return nullptr;
        }
        renderer->draw(text);
        return text;
    }
    /**
     * @brief Get the shape without drawing or adding it to the renderer.
     * 
     * @return sf::Drawable* 
     */
    virtual sf::Text* getWithoutDrawing() {
        if(fontLoaded == false) {
            std::cout << "can't get text without a font";
            return nullptr;
        }
        return text;
    }

    TextBuilder& setString(std::string string) {
        text->setString(string);
        return *this;
    }

    TextBuilder& setFont(string fontName) {
        auto font = TextBuilder::loadFont(fontName);
        if (font) {
            text->setFont(*font.value());
            fontLoaded = true;
        } 
        return *this;
    }

    TextBuilder& setFont(sf::Font& font) {
        text->setFont(font);
        return *this;
    }

    TextBuilder setCharacterSize(int size) {
        text->setCharacterSize(size);
        return *this;
    }

    TextBuilder& setColor(sf::Color color) {
        text->setFillColor(color);
        return *this;
    }

    TextBuilder& addOutline(sf::Color color, int thickness) {
        text->setOutlineColor(color);
        text->setOutlineThickness(thickness);
        return *this;
    }

    TextBuilder& centerToRect(int rectX, int rectY, int rectWidth, int rectHeight) {
        string temp = text->getString().toAnsiString();
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

        text->setPosition(rect.left + rectX, rect.top  + rectY);
    }


    static std::optional<sf::Font*> loadFont(string fontName);

private:
    static map<string, sf::Font*> loadedFonts;
};


map<string, sf::Font*> TextBuilder::loadedFonts;//we don't need to delete the fonts because this static map will persist until the program ends.
std::optional<sf::Font*> TextBuilder::loadFont(string fontName) {
    if(TextBuilder::loadedFonts.count(fontName) == 0) {//if font not already in set, load it
        sf::Font* font = new sf::Font();
        const string path = "myRecources/Fonts/" + fontName + ".ttf";
        if (!font->loadFromFile(path)) {
            cout << "ERROR loading font with path " << path << " coult not be loaded. Text will not be displayed. \n";
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