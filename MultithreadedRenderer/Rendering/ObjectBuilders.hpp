#pragma once
#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

#include <iostream>
#include <optional>
#include <map>

class ShapeBuilder {
protected:
    sf::Shape* shape;
public:
    ShapeBuilder() {}
    /**
     * @brief Draw this shape permanently (by adding it to the renderers list of permanent objects) and change it's properties through the pointer.
     *
     * @return sf::Drawable*
     */
    virtual ts::Shape* build() {
        ts::Shape* tsShape = new  ts::Shape(shape);
        Renderer::addPermanentObject(tsShape);
        return tsShape;
    }
    /**
     * @brief Get the shape without drawing or adding it to the renderer.
     *
     * @return sf::Drawable*
     */
    virtual  ts::Shape* getWithoutDrawing() {
        return new  ts::Shape(shape);
    }
    /**
     * @brief Set the color of this shape.
     *
     * @param color
     * @return ShapeBuilder&
     */
    virtual ShapeBuilder& setColor(sf::Color color) {
        shape->setFillColor(color);
        return *this;
    }
    /**
     * @brief Add an outline for this shape.
     *
     * @param color
     * @return ShapeBuilder&
     */
    virtual ShapeBuilder& addOutline(sf::Color color, int thickness) {
        shape->setOutlineColor(color);
        shape->setOutlineThickness(thickness);
        return *this;
    }

    /**
     * @brief Add a preexisting texture to this shape.
     *
     * @param texture
     * @return ShapeBuilder&
     */
    virtual ShapeBuilder& addTexture(sf::Texture& texture) {
        shape->setTexture(&texture);
        if (texture.isRepeated() == true) {
            auto bounds = shape->getGlobalBounds();
            sf::IntRect rect = sf::IntRect(bounds.left, bounds.top, bounds.width, bounds.height);
            shape->setTextureRect(rect);
        }//else stretch
        return *this;
    }


    /**
     * @brief Load a texture and add it to this shape.
     *
     * @param texturePath
     * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
     * @return ShapeBuilder&
     */
    virtual ShapeBuilder& addTexture(std::string texturePath, bool repeat) {
        sf::Texture texture = loadTexture(texturePath, repeat);
        shape->setTexture(&texture);
        if (texture.isRepeated() == true) {
            auto bounds = shape->getGlobalBounds();
            sf::IntRect rect = sf::IntRect(bounds.left, bounds.top, bounds.width, bounds.height);
            shape->setTextureRect(rect);
        }//else stretch
        return *this;
    }

private:
    sf::Texture loadTexture(std::string path, bool repeat) {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
        {
            std::cout << "failed to load texture of path '" << path << "'";
            exit(0);
        }
        if (repeat == true) {
            sf::Image img = texture.copyToImage();
            if (texture.loadFromImage(img) == false) {
                std::cout << "failed to load texture image of path '" << path << "'";
                exit(0);
            }

            texture.setRepeated(true);
        }
        return texture;
    }
};

class CircleBuilder : public ShapeBuilder {
public:
    CircleBuilder(int x, int y, int radius) {
        shape = new sf::CircleShape(radius);
        shape->setPosition(x, y);
    }
};

class RectBuilder : public ShapeBuilder {
public:
    RectBuilder(int x, int y, int width, int height) {
        shape = new sf::RectangleShape(sf::Vector2f(width, height));
        shape->setPosition(x, y);
    }
};

class TextBuilder {
protected:
    sf::Text* text;
    bool fontLoaded = false;
public:
    /**
     * @brief Construct a new Text Builder object. If the font for the text can't be loaded, the built text will be a nullptr.
     *
     * @param renderer used for adding the text to the renderer's permanent objects or drawing them once.
     * @param x
     * @param y
     */
    TextBuilder(int x, int y) {
        text = new sf::Text();
        text->setPosition(x, y);

        //default values
        setFont("calibri");
    }
    /**
     * @brief Permanently draw this text.
     *
     * @return sf::Drawable*
     */
    virtual ts::Text* build() {
        if (fontLoaded == false) {
            std::cout << "can't build or get text without a font";
            return nullptr;
        }
        ts::Text* tsText = new ts::Text(this->text);
        Renderer::addPermanentObject(tsText);
        return tsText;
    }
    /**
     * @brief Get the shape without drawing or adding it to the renderer.
     *
     * @return sf::Drawable*
     */
    virtual ts::Text* getWithoutDrawing() {
        if (fontLoaded == false) {
            std::cout << "can't get text without a font";
            return nullptr;
        }
        return new ts::Text(this->text);
    }

    TextBuilder& setString(std::string string) {
        text->setString(string);
        return *this;
    }

    TextBuilder& setFont(std::string fontName) {
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

    TextBuilder& centerToRect(int rectX, int rectY, int rectWidth, int rectHeight);


    static std::optional<sf::Font*> loadFont(std::string fontName);

private:
    static std::map<std::string, sf::Font*> loadedFonts;
};


