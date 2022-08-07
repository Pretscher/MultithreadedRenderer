#pragma once
#include "ThreadSafeObjects.hpp"
#include "Renderer.hpp"

#include <iostream>
#include <optional>
#include <map>




class CircleBuilder {
private:
	sf::CircleShape* circle;
public:
    CircleBuilder(float x, float y, float radius) : circle(new sf::CircleShape(radius)) {
        circle->setPosition(x, y);
    }
	
    ts::Circle* build() {
        ts::Circle* tsCircle = new ts::Circle(circle);
        Renderer::addPermanentObject(tsCircle);
        return tsCircle;
    }

    ts::Circle* getWithoutDrawing() {
        return new  ts::Circle(circle);
    }
	

    CircleBuilder& setColor(sf::Color color) {
        circle->setFillColor(color);
        return *this;
    }
    /**
     * @brief Add an outline for this shape.
     *
     * @param color
     * @return ShapeBuilder&
     */
    CircleBuilder& addOutline(sf::Color color, float thickness) {
        circle->setOutlineColor(color);
        circle->setOutlineThickness(thickness);
        return *this;
    }

    /**
     * @brief Load a texture and add it to this shape.
     *
     * @param texturePath
     * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
     * @return ShapeBuilder&
     */
    CircleBuilder& addTexture(std::string texturePath, bool repeat) {
        Renderer::queueTextureLoading(texturePath, repeat, circle);
        return *this;
    }
};

class RectBuilder {
private:
    sf::RectangleShape* rect;
    std::string texturePath;
public:
    RectBuilder(float x, float y, float width, float height) {
        rect = new sf::RectangleShape(sf::Vector2f((float)width, (float)height));
        rect->setPosition(x, y);
    }

    ts::Rect* build() {
        ts::Rect* tsShape = new ts::Rect(rect);
        Renderer::addPermanentObject(tsShape);
        return tsShape;
    }
     ts::Rect* getWithoutDrawing() {
        return new  ts::Rect(rect);
    }



     RectBuilder& setColor(sf::Color color) {
         rect->setFillColor(color);
         return *this;
     }
     /**
      * @brief Add an outline for this shape.
      *
      * @param color
      * @return ShapeBuilder&
      */
     RectBuilder& addOutline(sf::Color color, float thickness) {
         rect->setOutlineColor(color);
         rect->setOutlineThickness(thickness);
         return *this;
     }


     /**
      * @brief Load a texture and add it to this shape.
      *
      * @param texturePath
      * @param repeat Specifies wether the texture should be repeated or stretched if it is too small for the shape.
      * @return ShapeBuilder&
      */
     RectBuilder& addTexture(std::string texturePath, bool repeat) {
         Renderer::queueTextureLoading(texturePath, repeat, rect);
         return *this;
     }
};

class LineBuilder {
private:
    sf::RectangleShape* line;
public:
    LineBuilder(float x1, float y1, float x2, float y2) {
        float dX = x2 - x1;
        float dY = y2 - y1;
		
        float defaultThickness = 1.0f;
        float rot = atan2(dY, dX) * 57.2958f;
        line = new sf::RectangleShape(sf::Vector2f(sqrt(abs(dX) * abs(dX) + abs(dY) * abs(dY)), defaultThickness));
        line->setOrigin(0, defaultThickness / 2);
        line->setRotation(rot);
        line->setPosition(x1, y1);
    }
    
    LineBuilder& setThickness(float thickness) {
        line->setScale(line->getScale().x, thickness);
        line->setOrigin(0, thickness / 2);
        return *this;
    }

    ts::Line* build() {
        ts::Line* tsShape = new  ts::Line(line);
        Renderer::addPermanentObject(tsShape);
        return tsShape;
    }
    ts::Line* getWithoutDrawing() {
        return new  ts::Line(line);
    }

    LineBuilder& setColor(sf::Color color) {
        line->setFillColor(color);
        return *this;
    }
    /**
     * @brief Add an outline for this shape.
     *
     * @param color
     * @return ShapeBuilder&
     */
    LineBuilder& addOutline(sf::Color color, float thickness) {
        line->setOutlineColor(color);
        line->setOutlineThickness(thickness);
        return *this;
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
    TextBuilder(float x, float y) {
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
    ts::Text* build() {
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
    ts::Text* getWithoutDrawing() {
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

    TextBuilder& setCharacterSize(int size) {
        text->setCharacterSize(size);
        return *this;
    }

    TextBuilder& setColor(sf::Color color) {
        text->setFillColor(color);
        return *this;
    }

    TextBuilder& addOutline(sf::Color color, float thickness) {
        text->setOutlineColor(color);
        text->setOutlineThickness(thickness);
        return *this;
    }

    TextBuilder& centerToRect(float rectX, float rectY, float rectWidth, float rectHeight);

private:
    static std::optional<sf::Font*> loadFont(std::string fontName);
    static std::map<std::string, sf::Font*> loadedFonts;
};


